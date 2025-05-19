// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerBase.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/WidgetComponent.h"
#include "PlayerWidgetComponent.h"
#include "PlayerStatComponent.h"
#include "PlayerHpBar.h"
#include "Animation/AnimationAsset.h"
#include "Animation/AnimInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "PlayerDefaultController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISense_Hearing.h"
#include "DrawDebugHelpers.h"

// Object Plugin
#include "SzComponents/Interaction.h"

// TODO: Delete Debug Library
#include "Kismet/KismetSystemLibrary.h"


// Sets default values
APlayerBase::APlayerBase()
{
	// player controller setting
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// capsule setting
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.f);
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("Pawn"));

	// movement setting
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0, 500, 0);
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 200.f;
	GetCharacterMovement()->MaxWalkSpeedCrouched = 80.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 0.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// mesh setting
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -50.f), FRotator(0.f, -90.f, 0.f));
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	GetMesh()->SetCollisionProfileName(TEXT("CharacterMesh"));

	// Create SpringArm Component & Setting
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 300.0f;
	SpringArm->bUsePawnControlRotation = true;

	// Create Camera Component & Setting
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);
	Camera->bUsePawnControlRotation = false;

	ReferenceSetting();

	// Stat
	Stat = CreateDefaultSubobject<UPlayerStatComponent>(TEXT("Stat"));

	// UI Widget
	GroggyWidget = CreateDefaultSubobject<UPlayerWidgetComponent>(TEXT("Widget"));
	GroggyWidget->SetupAttachment(GetMesh());
}

// Called when the game starts or when spawned
void APlayerBase::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		if (ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer())
		{
			if (UEnhancedInputLocalPlayerSubsystem* SubSystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
			{
				SubSystem->AddMappingContext(InputMappingContext, 0);
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerController is not allocation."), true, true, FLinearColor(0.0f, 0.66f, 1.0f), 5);
	}

	// Widget Setting
	if (GroggyWidget->GetWidgetClass())
	{
		GroggyWidget->SetWidgetSpace(EWidgetSpace::Screen);
		GroggyWidget->SetDrawSize(FVector2D(150.f, 80.f));
		GroggyWidget->SetRelativeLocation(FVector(0, 0, 0));
		GroggyWidget->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GroggyWidget->SetVisibility(false);
	}
}

// Called every frame
void APlayerBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	APlayerDefaultController* PlayerController = Cast<APlayerDefaultController>(GetController());

	if (!PlayerController || !PlayerController->Perception)
		return;

	// 현재 퍼셉션 컴포넌트에 인지된 오브젝트 리스트를 가져옴
	TArray<AActor*> PerceptionActors;
	PerceptionActors = PlayerController->PerceptionActors;
	
	// 화면상에서 가까운 오브젝트 판별
	FVector Start = Camera->GetComponentLocation();
	FVector Direction = Camera->GetForwardVector();
	Direction.Normalize();

	// 최소거리 설정
	float MinDistance = PlayerController->SightConfig->SightRadius - 100.f;

	// 가장 가까운 오브젝트를 찾고 지정해줌
	for (AActor* Actor : PerceptionActors)
	{

		//UObject* InteractiveObject = Cast<UObject>(Actor);
		//if (!InteractiveObject) continue; // 없으면 넘김

		FVector ToPoint = Actor->GetActorLocation() - Start;

		// 직선과 점 사이의 거리: |Dir x VecToPoint| / |Dir|
		float DistanceFromLine = FVector::CrossProduct(Direction, ToPoint).Size();

		if (DistanceFromLine < MinDistance)
		{
			/*if (NearestInteractiveObject)
			{
				if (NearestInteractiveObject != Actor)
				{
					NearestInteractiveObject->WidgetComponent->SetWorldScale3D(FVector(2.0f));
				}
			}*/

			// 가까운 오브젝트가 있을 때
			MinDistance = DistanceFromLine;
			NearestInteractiveObject = Actor;
		}
	}

	if (NearestInteractiveObject)
	{

		//NearestInteractiveObject->WidgetComponent->SetWorldScale3D(FVector(2.0f));
	}
}

void APlayerBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// 체력 0 일시 실행되는 함수
	Stat->OnHpZero.AddUObject(this, &APlayerBase::SetGroggy);
}

// Called to bind functionality to input
void APlayerBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);

	// Move Action
	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerBase::Move);
	// Look Action
	EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerBase::Look);
	// Run Action
	EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Started, this, &APlayerBase::Run);
	EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Completed, this, &APlayerBase::StopRun);
	// Crouch Action
	EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Triggered, this, &APlayerBase::PlayerCrouch);
	// Hacking Action
	EnhancedInputComponent->BindAction(HackingAction, ETriggerEvent::Triggered, this, &APlayerBase::Hacking);
	// Interactive Action
	EnhancedInputComponent->BindAction(InteractiveAction, ETriggerEvent::Triggered, this, &APlayerBase::Interactive);
	// Inventory Action
	//EnhancedInputComponent->BindAction(InventoryAction, ETriggerEvent::Triggered, this, &APlayerCharacter::OpenInventory);
}

void APlayerBase::SetGroggy()
{
	// Groggy Widget 활성화
	GroggyWidget->SetVisibility(true);
	// 이동속도 0으로 설정
	GetCharacterMovement()->MaxWalkSpeed = 0.f;
}

void APlayerBase::TakeDamage(float Damage)
{
	Stat->ApplyDamage(Damage);
}

void APlayerBase::SetupCharacterWidget(UMyPlayerUserWidget* UserWidget)
{
	UPlayerHpBar* PlayerHpBar = Cast<UPlayerHpBar>(UserWidget);

	if (PlayerHpBar)
	{
		PlayerHpBar->SetMaxHp(Stat->GetMaxHp());
	}
}

void APlayerBase::Move(const FInputActionValue& Value)
{
	if (Controller != nullptr)
	{
		FVector2D MovementVector = Value.Get<FVector2D>();

		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void APlayerBase::Look(const FInputActionValue& Value)
{
	if (Controller != nullptr)
	{
		FVector2D LookAxisVector = Value.Get<FVector2D>();

		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void APlayerBase::Run(const FInputActionValue& Value)
{
	if (!HasAuthority())
	{
		GetCharacterMovement()->MaxWalkSpeed = 800.f;
	}
	C2S_SetMaxWalkSpeed(800.f);
}

void APlayerBase::StopRun(const FInputActionValue& Value)
{
	if (!HasAuthority())
	{
		GetCharacterMovement()->MaxWalkSpeed = 200.f;
	}
	C2S_SetMaxWalkSpeed(200.f);
}

void APlayerBase::PlayerCrouch(const FInputActionValue& Value)
{
	if (bIsCrouched)
	{
		UnCrouch();
	}
	else
	{
		Crouch();
	}
}

void APlayerBase::Hacking(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Log, TEXT("Hacking Start"));
}

void APlayerBase::Interactive(const FInputActionValue& Value)
{
	if (NearestInteractiveObject)
	{
		C2S_Interactive(NearestInteractiveObject);
	}
}

void APlayerBase::C2S_Interactive_Implementation(UObject* interact)
{
	if (nullptr == interact)
	{
		UKismetSystemLibrary::PrintString(this, TEXT("ABaseObject Interact is nullptr"));
		return;
	}
	
	if (interact->GetClass()->ImplementsInterface(UInteraction::StaticClass()))
		IInteraction::Execute_OnInteract(interact, this);
}

void APlayerBase::C2S_SetMaxWalkSpeed_Implementation(float Speed)
{
	GetCharacterMovement()->MaxWalkSpeed = Speed;
}

void APlayerBase::ReferenceSetting()
{
	// Set the default input mapping context
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> InputMappingContextRef(TEXT("/Game/Project_TPT/Assets/Input/Player/IMC_PlayerIMC.IMC_PlayerIMC"));
	if (InputMappingContextRef.Object)
	{
		InputMappingContext = InputMappingContextRef.Object;
	}

	// Set the default values for the input actions
	static ConstructorHelpers::FObjectFinder<UInputAction> MoveActionRef(TEXT("/Game/Project_TPT/Assets/Input/Player/Actions/IA_Move.IA_Move"));
	if (MoveActionRef.Object)
	{
		MoveAction = MoveActionRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> LookActionRef(TEXT("/Game/Project_TPT/Assets/Input/Player/Actions/IA_Look.IA_Look"));
	if (LookActionRef.Object)
	{
		LookAction = LookActionRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> RunActionRef(TEXT("/Game/Project_TPT/Assets/Input/Player/Actions/IA_Run.IA_Run"));
	if (RunActionRef.Object)
	{
		RunAction = RunActionRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> CrouchActionRef(TEXT("/Game/Project_TPT/Assets/Input/Player/Actions/IA_Crouch.IA_Crouch"));
	if (CrouchActionRef.Object)
	{
		CrouchAction = CrouchActionRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> HackingActionRef(TEXT("/Game/Project_TPT/Assets/Input/Player/Actions/IA_Hacking.IA_Hacking"));
	if (HackingActionRef.Object)
	{
		HackingAction = HackingActionRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InteractiveActionRef(TEXT("/Game/Project_TPT/Assets/Input/Player/Actions/IA_Interactive.IA_Interactive"));
	if (InteractiveActionRef.Object)
	{
		InteractiveAction = InteractiveActionRef.Object;
	}

	//static ConstructorHelpers::FObjectFinder<UInputAction> InventoryActionRef(TEXT("/Game/Project_TPT/Assets/Input/Player/Actions/IA_Inventory.IA_Inventory"));
	//if (InventoryActionRef.Object)
	//{
	//	InventoryAction = InventoryActionRef.Object;
	//}
}