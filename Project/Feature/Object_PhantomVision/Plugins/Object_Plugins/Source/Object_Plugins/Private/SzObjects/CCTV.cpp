// Fill out your copyright notice in the Description page of Project Settings.


#include "SzObjects/CCTV.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "SzComponents/HackableComponent.h"
#include "SzComponents/NoiseComponent.h"
#include "SzComponents/CCTVManager.h"
#include "PhantomTwinsGameState.h"
#include "Blueprint/UserWidget.h"
#include "SzUI/PhantomVisionWidget.h"
#include "SzComponents/OutlineComponent.h"
#include "Net/UnrealNetwork.h"


ACCTV::ACCTV()
{
	PrimaryActorTick.bCanEverTick = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = -60.0f;
	SpringArm->bUsePawnControlRotation = false;
	SpringArm->bInheritPitch = false;
	SpringArm->bInheritYaw = false;
	SpringArm->bInheritRoll = false;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComp->SetupAttachment(SpringArm);
	CameraComp->bUsePawnControlRotation = false;

	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	AutoPossessPlayer = EAutoReceiveInput::Disabled;

	CurrentHackingPawn = nullptr;

	static ConstructorHelpers::FObjectFinder<UInputMappingContext> InputMappingContextRef(TEXT("/Game/Project_TPT/Assets/Input/Object/IMC_CCTV.IMC_CCTV"));
	InputMappingContext = InputMappingContextRef.Object;

	static ConstructorHelpers::FObjectFinder<UInputMappingContext> PlayerMappingContextRef(TEXT("/Game/Project_TPT/Assets/Input/Player/IMC_PlayerIMC.IMC_PlayerIMC"));
	PlayerMappingContext = PlayerMappingContextRef.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> IA_TurnRef(TEXT("/Game/Project_TPT/Assets/Input/Object/IA_Turn.IA_Turn"));
	IA_Turn = IA_TurnRef.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> IA_ExitRef(TEXT("/Game/Project_TPT/Assets/Input/Object/IA_Exit.IA_Exit"));
	IA_Exit = IA_ExitRef.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> IA_PrevRef(TEXT("/Game/Project_TPT/Assets/Input/Object/IA_Prev.IA_Prev"));
	IA_Prev = IA_PrevRef.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> IA_NextRef(TEXT("/Game/Project_TPT/Assets/Input/Object/IA_Next.IA_Next"));
	IA_Next = IA_NextRef.Object;
}

void ACCTV::BeginPlay()
{
	Super::BeginPlay();

	if (!HackingComp)
	{
		HackingComp = FindComponentByClass<UHackableComponent>();
		if (!HackingComp)
		{
			UE_LOG(LogTemp, Warning, TEXT("No HackingComp found on %s"), *GetName());
		}
	}

	if (!NoiseComp)
	{
		NoiseComp = FindComponentByClass<UNoiseComponent>();
		if (!NoiseComp)
		{
			UE_LOG(LogTemp, Warning, TEXT("No NoiseComp found on %s"), *GetName());
		}
	}

	// CCTVManager에 CCTV 추가
	APhantomTwinsGameState* gameState = Cast<APhantomTwinsGameState>(GetWorld()->GetGameState());
	
	if (gameState && gameState->GetCCTVManager())
	{
		gameState->GetCCTVManager()->AddCCTV(CCTVID, this);
	}

}

void ACCTV::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const float CurrentTime = GetWorld()->GetTimeSeconds();

	// 현재 해킹 중인 플레이어가 있을 때만 UpdateHackingProgress 호출
	if (CurrentHackingPawn && HackingComp->bIsHacking)
	{
		HackingComp->UpdateHackingProgress(CurrentHackingPawn, CurrentTime);
	}

	if (CurrentHackingPawn && NoiseComp && NoiseComp->bIsHacking)
	{
		NoiseComp->UpdateHackingProgress(CurrentHackingPawn, CurrentTime);
	}

	// 해킹된 상태에서 유지 시간이 지나면 초기화 (단, bKeepHacked가 false일 때만)
	if (HackingComp->bIsHacked && !HackingComp->bKeepHacked &&
		(CurrentTime - HackingComp->HackingStartTime >= HackingComp->HackedDuration))
	{
		HackingComp->CheckHackReset();
		CurrentHackingPawn = nullptr; // 해킹이 리셋되면 현재 해킹 플레이어도 초기화
	}
}

void ACCTV::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (IA_Turn) EnhancedInput->BindAction(IA_Turn, ETriggerEvent::Triggered, this, &ACCTV::Turn);
		if (IA_Exit) EnhancedInput->BindAction(IA_Exit, ETriggerEvent::Triggered, this, &ACCTV::Exit);
		if (IA_Prev) EnhancedInput->BindAction(IA_Prev, ETriggerEvent::Triggered, this, &ACCTV::Prev);
		if (IA_Next) EnhancedInput->BindAction(IA_Next, ETriggerEvent::Triggered, this, &ACCTV::Next);
	}
}

void ACCTV::OnInteractSever_Implementation(APawn* Interactor)
{
	APlayerController* PlayerController = Cast<APlayerController>(Interactor->GetController());
	if (!PlayerController || !CameraComp) return;

	if (IsActive && HackingComp->bIsHacked)	// 해킹comp
	{
		bIsInCCTVView ? ExitCCTVView(PlayerController) : EnterCCTVView(PlayerController);
	}
}

void ACCTV::OnInteractClient_Implementation(APawn* Interactor)
{

}

bool ACCTV::CanInteract_Implementation(const APawn* Interactor) const
{
	return HackingComp->bIsHacked && IsActive;	// 임시. 해킹이 되어있고 카드키가 있어야 Interact 가능
}

void ACCTV::SetWidgetVisibility_Implementation(bool Visible)
{
	ABaseObject::SetWidgetVisibility_Implementation(Visible);

}

void ACCTV::OnHackingStartedServer_Implementation(APawn* Interactor)
{
	UE_LOG(LogTemp, Log, TEXT("ACCTV::OnHackingStarted Server"));

	//// 현재 해킹 중인 플레이어 저장
	CurrentHackingPawn = Interactor;

	HackingComp->HackingStarted(Interactor);

	if (NoiseComp)
	{
		NoiseComp->HackingStarted(Interactor);
	}
}

void ACCTV::OnHackingStartedClient_Implementation(APawn* Interactor)
{
	UE_LOG(LogTemp, Log, TEXT("ACCTV::OnHackingStarted Client"));


	// 현재 해킹 중인 플레이어 저장
	CurrentHackingPawn = Interactor;

	HackingComp->HackingStarted(Interactor);

	if (NoiseComp)
	{
		NoiseComp->HackingStarted(Interactor);
	}
}

void ACCTV::OnHackingCompletedServer_Implementation(APawn* Interactor)
{
	UE_LOG(LogTemp, Log, TEXT("ACCTV::OnHackingCompleted Server"));

	// 해킹을 시작한 플레이어와 완료하는 플레이어가 같은지 확인
	if (CurrentHackingPawn != Interactor) return;

	HackingComp->HackingCompleted(Interactor);

	if (NoiseComp)
	{
		NoiseComp->HackingCompleted(Interactor);
	}

	// 해킹 완료 후 현재 해킹 플레이어 초기화
	CurrentHackingPawn = nullptr;

	// HackedIDSet에서 CCTV(자신) 추가
	if (HackingComp->bIsHacked == true)
	{
		APhantomTwinsGameState* gameState = Cast<APhantomTwinsGameState>(GetWorld()->GetGameState());

		if (gameState && gameState->GetCCTVManager())
		{
			gameState->GetCCTVManager()->AddHackedCCTV(CCTVID);
		}
	}
}

void ACCTV::OnHackingCompletedClient_Implementation(APawn* Interactor)
{
	UE_LOG(LogTemp, Log, TEXT("ACCTV::OnHackingCompleted Client"));


}

bool ACCTV::CanBeHacked_Implementation() const
{
	if (!HackingComp) return false;

	return !(HackingComp->bIsHacked) && !(HackingComp->bIsHacking);
}

void ACCTV::ClearHacking_Implementation()
{
	// 해킹 초기화
	HackingComp->CheckHackReset(CurrentHackingPawn);

	if (NoiseComp)
	{
		NoiseComp->CheckHackReset(CurrentHackingPawn);
	}

	CurrentHackingPawn = nullptr;

	// HackedIDSet에서 CCTV(자신) 제거
	if (HackingComp->bIsHacked == false)
	{
		APhantomTwinsGameState* gameState = Cast<APhantomTwinsGameState>(GetWorld()->GetGameState());

		if (gameState && gameState->GetCCTVManager())
		{
			gameState->GetCCTVManager()->RemoveHackedCCTV(CCTVID);
		}
	}
}

void ACCTV::Turn(const FInputActionValue& Value)
{
	if (!Controller || !bIsInCCTVView) return;
	APlayerController* PC = Cast<APlayerController>(Controller);
	if (!PC) return;

	FVector2D Input = Value.Get<FVector2D>();
	FRotator ControlRot = PC->GetControlRotation();
	ControlRot.Yaw += Input.X;
	ControlRot.Pitch += Input.Y;

	FRotator DeltaRot = (ControlRot - BaseControlRotation).GetNormalized();
	DeltaRot.Pitch = FMath::Clamp(DeltaRot.Pitch, -MaxPitch, MaxPitch);
	DeltaRot.Yaw = FMath::Clamp(DeltaRot.Yaw, -MaxYaw, MaxYaw);
	DeltaRot.Roll = 0.0f;

	FRotator FinalRot = BaseControlRotation + DeltaRot;

	PC->SetControlRotation(FinalRot);
	SetActorRotation(FinalRot);
}

void ACCTV::Exit(const FInputActionValue& Value)
{
	if (bIsInCCTVView && Controller)
	{
		if (APlayerController* PC = Cast<APlayerController>(Controller))
		{
			ExitCCTVView(PC);
		}
	}
}

void ACCTV::Prev(const FInputActionValue& Value)
{
	APhantomTwinsGameState* gameState = Cast<APhantomTwinsGameState>(GetWorld()->GetGameState());

	if (gameState && gameState->GetCCTVManager())
	{
		ACCTV* prevCCTV = gameState->GetCCTVManager()->GetPrevHackedCCTV(CCTVID);
		APlayerController* PC = Cast<APlayerController>(Controller);

		if (PC)
		{
			ExitCCTVView(PC);

			if (prevCCTV)
			{
				prevCCTV->EnterCCTVView(PC);
			}
		}
	}
}

void ACCTV::Next(const FInputActionValue& Value)
{
	APhantomTwinsGameState* gameState = Cast<APhantomTwinsGameState>(GetWorld()->GetGameState());

	if (gameState && gameState->GetCCTVManager())
	{
		ACCTV* nextCCTV = gameState->GetCCTVManager()->GetNextHackedCCTV(CCTVID);
		APlayerController* PC = Cast<APlayerController>(Controller);


		if (PC)
		{
			ExitCCTVView(PC);

			if (nextCCTV)
			{
				nextCCTV->EnterCCTVView(PC);
			}
		}
	}
}

void ACCTV::EnterCCTVView(APlayerController* PlayerController)
{
	PreviousViewTarget = PlayerController->GetViewTarget();
	PreviousPawn = PlayerController->GetPawn();
	PlayerController->Possess(this);

	BaseControlRotation = GetActorRotation();
	PlayerController->SetControlRotation(BaseControlRotation);

	bIsInCCTVView = true;

	if (ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer())
	{
		InputSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
		if (InputSubsystem && InputMappingContext && PlayerMappingContext)
		{
			InputSubsystem->RemoveMappingContext(PlayerMappingContext);
			InputSubsystem->AddMappingContext(InputMappingContext, 0);
		}
	}

	PlayerController->SetInputMode(FInputModeGameOnly());
	EnableInput(PlayerController);

	// 위젯 생성 및 뷰포트 추가
	if (PhantomVisionWidget && !PhantomVisionUI)
	{
		PhantomVisionUI = CreateWidget<UUserWidget>(PlayerController, PhantomVisionWidget);
		if (PhantomVisionUI)
		{
			PhantomVisionUI->AddToViewport();

			if (UPhantomVisionWidget* Widget = Cast<UPhantomVisionWidget>(PhantomVisionUI))
			{
				Widget->SetCCTVIDTxt(CCTVID);
			}
		}
	}

	SetActorsOutlines(true);
}

void ACCTV::ExitCCTVView(APlayerController* PlayerController)
{
	if (PreviousPawn)
	{
		PlayerController->Possess(PreviousPawn);
		PreviousPawn->EnableInput(PlayerController);
	}

	bIsInCCTVView = false;

	if (InputSubsystem && InputMappingContext && PlayerMappingContext)
	{
		InputSubsystem->RemoveMappingContext(InputMappingContext);
		InputSubsystem->AddMappingContext(PlayerMappingContext, 0);
	}

	PlayerController->SetInputMode(FInputModeGameOnly());

	// 위젯 뷰포트에서 제거
	if (PhantomVisionUI)
	{
		PhantomVisionUI->RemoveFromParent();
		PhantomVisionUI = nullptr;
	}

	SetActorsOutlines(false);
}

void ACCTV::SetActorsOutlines(bool bActive)
{
	UWorld* World = GetWorld();
	if (!World) return;

	// 레벨에 존재하는 모든 액터 가져오기
	TArray<AActor*> AllActors;
	UGameplayStatics::GetAllActorsOfClass(World, AActor::StaticClass(), AllActors);

	for (AActor* Actor : AllActors)
	{
		if (!IsValid(Actor)) continue; // Actor nullptr 및 유효성 체크

		// 액터에 UOutlineComponent가 붙어 있는지 확인
		UOutlineComponent* FoundOutlineComp = Actor->FindComponentByClass<UOutlineComponent>();
		if (IsValid(FoundOutlineComp))
		{
			FoundOutlineComp->SetOutline(bActive);
		}
	}
}
