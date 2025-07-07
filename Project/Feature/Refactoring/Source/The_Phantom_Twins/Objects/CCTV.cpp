// Fill out your copyright notice in the Description page of Project Settings.


#include "CCTV.h"
#include "CCTVManager.h"
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
#include "../PhantomTwinsGameState.h"
#include "SzUI/PhantomVisionWidget.h"
#include "Net/UnrealNetwork.h"

#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Blueprint/UserWidget.h"
#include "SzComponents/InteractableComponent.h"
#include "SzComponents/OutlineComponent.h"

//AI Perception
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISense_Hearing.h"
#include "Perception/AISenseConfig_Hearing.h"

ACCTV::ACCTV()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	// Root Scene
	RootSceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("MeshComponent"));
	SetRootComponent(RootSceneComp);

	// Mesh
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComp->SetCollisionProfileName(TEXT("OverlapAll"));

	// AIPerception과 player안의 sphere만 감지하는 Object
	SphereCollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	SphereCollisionComp->SetupAttachment(MeshComp);
	SphereCollisionComp->SetSphereRadius(50.0f);
	SphereCollisionComp->SetCollisionObjectType(ECC_GameTraceChannel1); // Object Type 설정

	// Outline
	OutlineComp = CreateDefaultSubobject<UOutlineComponent>(TEXT("OutlineComponent"));

	// 위젯 컴포넌트는 항상 생성하지만, 사용 여부에 따라 설정/표시 여부 제어
	NearWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("NearObjectWidget"));
	NearWidgetComp->SetupAttachment(MeshComp);
	NearWidgetComp->SetWidgetSpace(EWidgetSpace::Screen);
	NearWidgetComp->SetDrawSize(FVector2D(10, 10));
	NearWidgetComp->SetRelativeLocation(FVector(0, 0, 100));
	NearWidgetComp->SetVisibility(false); // 기본은 비활성화

	// AI Perception
	StimuliSource = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("StimuliSource"));
	StimuliSource->bAutoRegister = true;
	StimuliSource->RegisterForSense(UAISense_Sight::StaticClass());
	StimuliSource->RegisterForSense(UAISense_Hearing::StaticClass());

	// "Object" 태그 추가
	Tags.Add(FName("Interactable"));

	HackingComp = CreateDefaultSubobject<UHackableComponent>(TEXT("HackableComponent"));
	NoiseComp = CreateDefaultSubobject<UNoiseComponent>(TEXT("NoiseComponent"));

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	//SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = -60.0f;
	SpringArm->bUsePawnControlRotation = false;
	SpringArm->bInheritPitch = true;
	SpringArm->bInheritYaw = true;
	SpringArm->bInheritRoll = true;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComp->SetupAttachment(SpringArm);
	CameraComp->bUsePawnControlRotation = false;

	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	AutoPossessPlayer = EAutoReceiveInput::Disabled;

	CurrentHackingPawn = nullptr;


	static ConstructorHelpers::FObjectFinder<UInputMappingContext> PlayerMappingContextRef(TEXT("/Game/Project_TPT/Assets/Input/Player/IMC_PlayerIMC.IMC_PlayerIMC"));
	PlayerMappingContext = PlayerMappingContextRef.Object;
	InputMappingContext = PlayerMappingContextRef.Object;

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

	// Near Object Widget
	if (NearWidgetComp)
	{
		if (NearWidgetClass)
		{
			NearWidgetComp->SetWidgetClass(NearWidgetClass);
			NearWidgetComp->SetVisibility(false);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("WidgetClass is not set for %s"), *GetName());
		}
	}

	// Outline
	if (OutlineComp)
	{
		OutlineComp->SetOutline(false);
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

	// 해킹이 완료 되었고 noise 발생이 안되고 있을 경우
	if (HackingComp->bIsHacked && !NoiseComp->bNoise)
	{
		NoiseComp->StartNoise();	// tick에서 실행 시키면 안됨. 나중에 옮기기
	}

/*  CCTV는 되돌리기 전까지 계속 해킹 유지하니까 얘는 빼도 될듯  */
	// 해킹된 상태에서 유지 시간이 지나면 초기화 (단, bKeepHacked가 false일 때만)
	if (HackingComp->bIsHacked && !HackingComp->bKeepHacked &&
		(CurrentTime - HackingComp->HackingStartTime >= HackingComp->HackedDuration))
	{
		HackingComp->CheckHackReset(CurrentHackingPawn);
		NoiseComp->StopNoise();
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

void ACCTV::OnInteractServer_Implementation(const APawn* Interactor)
{
	APlayerController* PlayerController = Cast<APlayerController>(Interactor->GetController());
	if (!PlayerController || !CameraComp) return;

	if (IsActive && HackingComp->bIsHacked)	// 해킹comp
	{
		bIsInCCTVView ? ExitCCTVView(PlayerController) : EnterCCTVView(PlayerController);
	}
}

void ACCTV::OnHackingStartedServer_Implementation(const APawn* Interactor)
{
	UE_LOG(LogTemp, Log, TEXT("ACCTV::OnHackingStarted Server"));

	// 현재 해킹 중인 플레이어 저장
	CurrentHackingPawn = Interactor;

	HackingComp->HackingStarted(Interactor);
}


void ACCTV::OnHackingCompletedServer_Implementation(const APawn* Interactor)
{
	UE_LOG(LogTemp, Log, TEXT("ACCTV::OnHackingCompleted Server"));

	// 해킹을 시작한 플레이어와 완료하는 플레이어가 같은지 확인
	if (CurrentHackingPawn != Interactor) return;

	HackingComp->HackingCompleted(Interactor);

	// 해킹 완료 후 현재 해킹 플레이어 초기화
	CurrentHackingPawn = nullptr;

	// HackedIDSet에서 CCTV(자신) 추가
	if (HackingComp->bIsHacked == true)
	{
		APhantomTwinsGameState* gameState = Cast<APhantomTwinsGameState>(GetWorld()->GetGameState());

		if (gameState && gameState->GetCCTVManager())
		{
			gameState->GetCCTVManager()->SetHackedCCTV(CCTVID, true);
		}
	}
}

bool ACCTV::CanBeHacked_Implementation(const APawn* Interactor)
{
	SetWidgetVisible(bCanInteract);

	return !(HackingComp->bIsHacked) && !(HackingComp->bIsHacking);	// 해킹된 상태랑 해킹할 수 있는 상태는 반대.
}

void ACCTV::ClearHacking_Implementation(const APawn* Interactor)
{
	NoiseComp->StopNoise();

	// 해킹 초기화
	HackingComp->CheckHackReset(CurrentHackingPawn);

	CurrentHackingPawn = nullptr;

	// HackedIDSet에서 CCTV(자신) 제거
	if (HackingComp->bIsHacked == false)
	{
		APhantomTwinsGameState* gameState = Cast<APhantomTwinsGameState>(GetWorld()->GetGameState());

		if (gameState && gameState->GetCCTVManager())
		{
			gameState->GetCCTVManager()->SetHackedCCTV(CCTVID, false);
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
	C2S_Exit();
}

void ACCTV::Prev(const FInputActionValue& Value)
{
	APhantomTwinsGameState* gameState = Cast<APhantomTwinsGameState>(GetWorld()->GetGameState());

	if (gameState && gameState->GetCCTVManager())
	{
		ACCTV* prevCCTV = gameState->GetCCTVManager()->GetPrevHackedCCTV(CCTVID);
		if (!prevCCTV)
			return;
		C2S_ChangeCCTV(prevCCTV);

	}
}

void ACCTV::Next(const FInputActionValue& Value)
{
	APhantomTwinsGameState* gameState = Cast<APhantomTwinsGameState>(GetWorld()->GetGameState());

	if (gameState && gameState->GetCCTVManager())
	{
		ACCTV* nextCCTV = gameState->GetCCTVManager()->GetNextHackedCCTV(CCTVID);
		if (!nextCCTV)
			return;
		C2S_ChangeCCTV(nextCCTV);
	}
}

void ACCTV::EnterCCTVView(APlayerController* PlayerController)
{
	PreviousViewTarget = PlayerController->GetViewTarget();
	PreviousPawn = PlayerController->GetPawn();
	PlayerController->Possess(this);

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

	bSetWidgetDirtyFlag = true;
	OnRep_SetWidget();
	bSetOutlineDirtyFlag = true;
	OnRep_SetOutlines();

	APhantomTwinsGameState* gameState = Cast<APhantomTwinsGameState>(GetWorld()->GetGameState());
	if (gameState && gameState->GetCCTVManager())
	{
		gameState->GetCCTVManager()->SetUsedCCTV(CCTVID, true);
	}
}

void ACCTV::ExitCCTVView(APlayerController* PlayerController)
{
	if (PreviousPawn)
	{
		PlayerController->Possess(PreviousPawn);
		PreviousPawn->EnableInput(PlayerController);
		PreviousPawn = nullptr;
	}

	bIsInCCTVView = false;

	if (InputSubsystem && InputMappingContext && PlayerMappingContext)
	{
		InputSubsystem->RemoveMappingContext(InputMappingContext);
		InputSubsystem->AddMappingContext(PlayerMappingContext, 0);
	}

	PlayerController->SetInputMode(FInputModeGameOnly());

	bSetWidgetDirtyFlag = false;
	OnRep_SetWidget();

	APhantomTwinsGameState* gameState = Cast<APhantomTwinsGameState>(GetWorld()->GetGameState());
	if (gameState && gameState->GetCCTVManager())
	{
		gameState->GetCCTVManager()->SetUsedCCTV(CCTVID, false);
	}
}

void ACCTV::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACCTV, bSetWidgetDirtyFlag);
	DOREPLIFETIME(ACCTV, bSetOutlineDirtyFlag);
	DOREPLIFETIME(ACCTV, bIsInCCTVView);
	DOREPLIFETIME(ACCTV, IsActive);
}

void ACCTV::SetWidgetVisible(bool Visible)
{
	NearWidgetComp->SetVisibility(Visible);
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

void ACCTV::OnRep_SetWidget()
{
	if (!bSetWidgetDirtyFlag)
	{
		if (!PhantomVisionUI && !IsLocallyControlled())
		{
			if (!PhantomVisionUI)
				UE_LOG(LogCameraManger, Warning, TEXT("ACCTV: OnRep_SetWidget: PhantomVisionUI is nullptr.(bSetWidgetDirtyFlag is false)"));
			return;
		}
		PhantomVisionUI->RemoveFromParent();
	}
	else
	{
		if (!IsLocallyControlled())
		{
			return;
		}

		BaseControlRotation = GetActorRotation();

		APlayerController* PlayerController = Cast<APlayerController>(GetController());
		if (!PlayerController)
		{
			UE_LOG(LogCameraManger, Error, TEXT("ACCTV: OnRep_SetWidget: Cast To PlayerController Faild."));
			return;
		}
		PlayerController->SetControlRotation(BaseControlRotation);

		// 위젯 생성 및 뷰포트 추가
		if (PhantomVisionWidget)
		{
			if (!PhantomVisionUI)
			{
				PhantomVisionUI = CreateWidget<UPhantomVisionWidget>(PlayerController, PhantomVisionWidget);
				if (!PhantomVisionUI)
				{
					UE_LOG(LogCameraManger, Error, TEXT("ACCTV: OnRep_SetWidget: CreateWidget PhantomVisionUI Faild."));
				}
			}

			if (!PhantomVisionUI)
			{
				UE_LOG(LogCameraManger, Warning, TEXT("ACCTV: OnRep_SetWidget: PhantomVisionUI is nullptr.(bSetWidgetDirtyFlag is true)"));
				return;
			}

			if (bSetWidgetDirtyFlag)
			{
				PhantomVisionUI->AddToViewport();

				if (UPhantomVisionWidget* Widget = Cast<UPhantomVisionWidget>(PhantomVisionUI))
				{
					Widget->SetCCTVIDTxt(CCTVID);
				}
			}
		}
	}
}

void ACCTV::OnRep_SetOutlines()
{
	if (!bSetOutlineDirtyFlag)
	{
		SetActorsOutlines(bSetOutlineDirtyFlag);
	}
	else
	{
		if (IsLocallyControlled())
		{
			SetActorsOutlines(bSetOutlineDirtyFlag);
		}
	}
}

void ACCTV::C2S_Exit_Implementation()
{
	if (bIsInCCTVView && Controller)
	{
		if (APlayerController* PC = Cast<APlayerController>(Controller))
		{
			ExitCCTVView(PC);
			bSetOutlineDirtyFlag = false;
			OnRep_SetOutlines();
		}
	}
}

void ACCTV::C2S_ChangeCCTV_Implementation(ACCTV* nextCCTV)
{
	APlayerController* PC = Cast<APlayerController>(Controller);
	if (PC)
	{
		ExitCCTVView(PC);
		nextCCTV->EnterCCTVView(PC);
	}
}
