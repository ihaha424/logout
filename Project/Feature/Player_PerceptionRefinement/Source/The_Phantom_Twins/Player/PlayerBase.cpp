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

void APlayerBase::NearestObjectCheck()
{
	APlayerDefaultController* PlayerController = Cast<APlayerDefaultController>(GetController());

	if (!PlayerController)
		return;

	// 현재 퍼셉션 컴포넌트에 인지된 오브젝트 리스트를 가져옴
	TArray<AActor*> Objects = PlayerController->PerceptionActors;

	// 화면상에서 가까운 오브젝트 판별
	FVector Start = Camera->GetComponentLocation();
	FVector Direction = Camera->GetForwardVector();
	Direction.Normalize();

	// 최소거리 설정
	float MinDistance = PlayerController->SightConfig->SightRadius - 100.f;

	// 가장 가까운 오브젝트를 찾고 지정해줌
	for (AActor* Actor : Objects)
	{
		if (!Cast<IInteraction>(Actor))
			continue;

		FVector ToPoint = Actor->GetActorLocation() - Start;

		// 직선과 점 사이의 거리: |Dir x VecToPoint| / |Dir|
		float DistanceFromLine = FVector::CrossProduct(Direction, ToPoint).Size();

		if (DistanceFromLine < MinDistance)
		{
			if (NearestInteractiveObject)
			{
				// NearestInteractiveObject
			}

			// 가까운 오브젝트가 있을 때
			MinDistance = DistanceFromLine;
			NearestInteractiveObject = Actor;
			// NearestInteractiveObject->WidgetComponent->SetWorldScale3D(FVector(2.0f));
		}
	}
}

// Called when the game starts or when spawned
void APlayerBase::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PlayerController = CastChecked<APlayerController>(GetController());
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

	if (InvenWidgetClass)
	{
		InvenWidget = CreateWidget<UUserWidget>(PlayerController, InvenWidgetClass);

		if (InvenWidget)
		{
			InvenWidget->AddToViewport();
			InvenWidget->SetVisibility(ESlateVisibility::Hidden);
		}
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

	NearestObjectCheck();
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
	EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Triggered, this, &APlayerBase::Run);
	EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Completed, this, &APlayerBase::StopRun);
	// Crouch Action
	EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Triggered, this, &APlayerBase::PlayerCrouch);
	// Hacking Action
	EnhancedInputComponent->BindAction(HackingAction, ETriggerEvent::Triggered, this, &APlayerBase::Hacking);
	// Interactive Action
	EnhancedInputComponent->BindAction(InteractiveAction, ETriggerEvent::Triggered, this, &APlayerBase::Interactive);
	// Inventory Action
	EnhancedInputComponent->BindAction(InventoryAction, ETriggerEvent::Triggered, this, &APlayerBase::OpenInventory);
}

void APlayerBase::SetGroggy()
{
	// Groggy Widget 활성화
	GroggyWidget->SetVisibility(true);
	// 이동속도 0으로 설정
	GetCharacterMovement()->MaxWalkSpeed = 0.f;
	UE_LOG(LogTemp, Warning, TEXT("Character is Groggy!"));
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

	static ConstructorHelpers::FObjectFinder<UInputAction> InventoryActionRef(TEXT("/Game/Project_TPT/Assets/Input/Player/Actions/IA_Inven.IA_Inven"));
	if (InventoryActionRef.Object)
	{
		InventoryAction = InventoryActionRef.Object;
	}

	static ConstructorHelpers::FClassFinder<UUserWidget> InvenWidgetRef(TEXT("/Game/Project_TPT/Assets/Blueprints/Player/WB_Inventory.WB_Inventory_C"));
	if (InvenWidgetRef.Class)
	{
		InvenWidgetClass = InvenWidgetRef.Class;
	}
}

void APlayerBase::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	AddMovementInput(ForwardDirection, MovementVector.Y);
	AddMovementInput(RightDirection, MovementVector.X);
}

void APlayerBase::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	AddControllerYawInput(LookAxisVector.X);
	AddControllerPitchInput(LookAxisVector.Y);
}

void APlayerBase::Run(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Log, TEXT("Run"));
	GetCharacterMovement()->MaxWalkSpeed = 800.f; // 기본 걷기보다 빠르게 설정
}

void APlayerBase::StopRun(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Log, TEXT("Walk"));
	GetCharacterMovement()->MaxWalkSpeed = 200.f; // 걷기 속도로 복구
}

void APlayerBase::PlayerCrouch(const FInputActionValue& Value)
{
	if (bIsCrouched)
	{
		UnCrouch();
		UE_LOG(LogTemp, Log, TEXT("UnCrouch"));
	}
	else
	{
		Crouch();
		UE_LOG(LogTemp, Log, TEXT("Crouch"));
	}
}

void APlayerBase::Hacking(const FInputActionValue& Value)
{

	UE_LOG(LogTemp, Log, TEXT("Hacking Start"));
}

void APlayerBase::Interactive(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Log, TEXT("Interactive Object"));
	if (NearestInteractiveObject)
	{
		// 상호작용 된 오브젝트 로직 실행
		IInteraction::Execute_OnInteract(NearestInteractiveObject, this);

		if (IInteraction::Execute_GetPickedUp(NearestInteractiveObject))
		{
			InventoryObjects.Add(NearestInteractiveObject);
			AddItemToUI();
		}
	}
}

void APlayerBase::OpenInventory(const FInputActionValue& Value)
{
	APlayerController* PC = CastChecked<APlayerController>(GetController());
	
	if (!PC || !InvenWidget) return;

	bIsInventoryVisible = !bIsInventoryVisible;

	if (bIsInventoryVisible)
	{
		UE_LOG(LogTemp, Log, TEXT("OpenInventory"));

		InvenWidget->SetVisibility(ESlateVisibility::Visible);

		FInputModeGameAndUI InputMode;
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		InputMode.SetHideCursorDuringCapture(false);
		PC->SetInputMode(InputMode);

		PC->bShowMouseCursor = true;
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("CloseInventory"));
		InvenWidget->SetVisibility(ESlateVisibility::Hidden);

		FInputModeGameOnly InputMode;
		PC->SetInputMode(InputMode);

		PC->bShowMouseCursor = false;
	}
}

