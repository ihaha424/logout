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

	static ConstructorHelpers::FObjectFinder<UInputMappingContext> InputMappingContextRef(TEXT("/Game/Project_TPT/Assets/Input/Object/IMC_CCTV.IMC_CCTV"));
	InputMappingContext = InputMappingContextRef.Object;

	static ConstructorHelpers::FObjectFinder<UInputMappingContext> PlayerMappingContextRef(TEXT("/Game/Project_TPT/Assets/Input/Player/IMC_PlayerIMC.IMC_PlayerIMC"));
	PlayerMappingContext = PlayerMappingContextRef.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> IA_TurnRef(TEXT("/Game/Project_TPT/Assets/Input/Object/IA_Turn.IA_Turn"));
	IA_Turn = IA_TurnRef.Object;

	static ConstructorHelpers::FObjectFinder<UInputAction> IA_ExitRef(TEXT("/Game/Project_TPT/Assets/Input/Object/IA_Exit.IA_Exit"));
	IA_Exit = IA_ExitRef.Object;
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
}

void ACCTV::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const float CurrentTime = GetWorld()->GetTimeSeconds();

	HackingComp->UpdateHackingProgress(CurrentTime);
	NoiseComp->UpdateHackingProgress(CurrentTime);

	// 해킹된 상태에서 유지 시간이 지나면 초기화 (단, bKeepHacked가 false일 때만)
	if (HackingComp->bIsHacked && !HackingComp->bKeepHacked &&
		(CurrentTime - HackingComp->HackingStartTime >= HackingComp->HackedDuration))
	{
		HackingComp->CheckHackReset();
	}
}

void ACCTV::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (IA_Turn) EnhancedInput->BindAction(IA_Turn, ETriggerEvent::Triggered, this, &ACCTV::Turn);
		if (IA_Exit) EnhancedInput->BindAction(IA_Exit, ETriggerEvent::Triggered, this, &ACCTV::Exit);
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

bool ACCTV::CanInteract_Implementation(const APawn* Interactor) const
{
	return bHasKey;
}

void ACCTV::SetWidgetVisibility_Implementation(bool Visible)
{
	ABaseObject::SetWidgetVisibility_Implementation(Visible);

}

void ACCTV::OnHackingStarted_Implementation(APawn* Interactor)
{
	HackingComp->HackingStarted();
	NoiseComp->HackingStarted();
}

void ACCTV::OnHackingCompleted_Implementation(APawn* Interactor)
{
	HackingComp->HackingCompleted();
	NoiseComp->HackingCompleted();
}

bool ACCTV::CanBeHacked_Implementation() const
{
	return !(HackingComp->bIsHacked);	// 해킹된 상태랑 해킹할 수 있는 상태는 반대.
}

void ACCTV::ClearHacking_Implementation()
{
	// 해킹 초기화
	HackingComp->CheckHackReset();
	NoiseComp->CheckHackReset();
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
}
