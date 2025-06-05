// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerBase.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
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
#include "DrawDebugHelpers.h"

// Object Plugin
#include "SzComponents/Interaction.h"
#include "SzInterface/Hacking.h"

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

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	SphereComponent->SetupAttachment(RootComponent);
	SphereComponent->SetSphereRadius(500.0f);
	SphereComponent->SetRelativeLocation(FVector(0.f, 0.f, 50.f));
	// Only Overlap
	SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	// 충돌 채널 유형 설정
	SphereComponent->SetCollisionObjectType(ECC_WorldDynamic);
	// 모든 채널 충돌 무시
	SphereComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	// Object들과 Player까지 오버랩 이벤트 발생하도록
	SphereComponent->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	// 이 컴포넌트에서 Overlap 이벤트 호출 활성화
	SphereComponent->SetGenerateOverlapEvents(true);

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

bool APlayerBase::CheckActorInFront(AActor* TargetActor)
{
	if (!TargetActor) return false;

	USphereComponent* Sphere = TargetActor->FindComponentByClass<USphereComponent>();

	if (!Sphere) return false;

	FVector Start = GetActorLocation();
	FVector End = Sphere->GetComponentLocation();

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this); // 자기 자신은 무시

	bool bHit = GetWorld()->LineTraceSingleByChannel(
		Hit,
		Start,
		End,
		ECC_Visibility,
		Params
	);

#if WITH_EDITOR
	DrawDebugLine(GetWorld(), Start, End, Hit.GetActor() == TargetActor ? FColor::Blue : FColor::Silver, false, 1.0f, 0, 0.3f);
#endif

	// Ray가 정확히 TargetActor에 부딪혔는지 확인
	return bHit && Hit.GetActor() == TargetActor;
}

void APlayerBase::NearestObjectCheck()
{
	// 화면상에서 가까운 오브젝트 판별
	FVector Start = Camera->GetComponentLocation();
	FVector Direction = Camera->GetForwardVector();
	Direction.Normalize();

	// 최소거리 설정
	float MinDistance = SphereComponent->GetScaledSphereRadius();

	// 가장 가까운 오브젝트를 찾고 지정해줌
	for (AActor* Actor : InteractiveableObjects)
	{
		if (!(Cast<IInteraction>(Actor) || Cast<IHacking>(Actor)))
			continue;

		if (CheckActorInFront(Actor))
		{
			// Set Object UI
			if (UWidgetComponent* Widget = Actor->FindComponentByClass<UWidgetComponent>())
			{
				//UE_LOG(LogTemp, Log, TEXT("Widget On"));
				Widget->SetVisibility(true);
			}
		}
		else
		{
			// Set Object UI
			if (UWidgetComponent* Widget = Actor->FindComponentByClass<UWidgetComponent>())
			{
				//UE_LOG(LogTemp, Log, TEXT("Widget Off"));
				Widget->SetVisibility(false);
			}
			continue;
		}

		FVector ToPoint = Actor->GetActorLocation() - Start;

		// 직선과 점 사이의 거리: |Dir x VecToPoint| / |Dir|
		float DistanceFromLine = FVector::CrossProduct(Direction, ToPoint).Size();

		if (DistanceFromLine < MinDistance)
		{
			if (NearestInteractiveObject)
			{

			}
			// 가까운 오브젝트가 있을 때
			MinDistance = DistanceFromLine;
			NearestInteractiveObject = Actor;
		}
	}
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

	// Noise 발생
	NoiseTimer += DeltaTime;
	if (NoiseTimer >= NoiseInterval)
	{
		UE_LOG(LogTemp, Log, TEXT("Noise : %.2f"), CurrentNoise);
		MakeNoise(CurrentNoise, this, GetActorLocation());
		NoiseTimer = 0.f;
	}

	// Tick 또는 디버그용 함수 안에서
	FVector SphereLocation = SphereComponent->GetComponentLocation();
	float SphereRadius = SphereComponent->GetScaledSphereRadius();

	DrawDebugSphere(
		GetWorld(),
		SphereLocation,
		SphereRadius,
		32,
		FColor::Green,
		false,
		-1.f,
		0,
		2.f
	);

	NearestObjectCheck();
}

void APlayerBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// 체력 0 일시 실행되는 함수
	Stat->OnHpZero.AddUObject(this, &APlayerBase::SetGroggy);
}

void APlayerBase::NotifyActorBeginOverlap(AActor* Actor)
{
	Super::NotifyActorBeginOverlap(Actor);

	if (Actor->ActorHasTag("Player"))
	{
		// 
	}

	if (!Actor->ActorHasTag("Object"))
		return;

	UE_LOG(LogTemp, Warning, TEXT("Begin overlap"));
	
	if (HasAuthority())
	{
		S2C_UpdatePerceivedActor(Actor, true);
	}
}

void APlayerBase::NotifyActorEndOverlap(AActor* Actor)
{
	Super::NotifyActorEndOverlap(Actor);

	if (!Actor->ActorHasTag("Object"))
		return;
	UE_LOG(LogTemp, Warning, TEXT("End overlap"));
	if (HasAuthority())
	{
		S2C_UpdatePerceivedActor(Actor, false);
	}
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
	EnhancedInputComponent->BindAction(HackingAction, ETriggerEvent::Started, this, &APlayerBase::Hacking);
	EnhancedInputComponent->BindAction(HackingAction, ETriggerEvent::Completed, this, &APlayerBase::Hacking);
	EnhancedInputComponent->BindAction(HackingAction, ETriggerEvent::Canceled, this, &APlayerBase::StopHacking);
	// Interactive Action
	EnhancedInputComponent->BindAction(InteractiveAction, ETriggerEvent::Triggered, this, &APlayerBase::Interactive);
	// Inventory Action
	EnhancedInputComponent->BindAction(InventoryAction, ETriggerEvent::Triggered, this, &APlayerBase::OpenInventory);
}

void APlayerBase::SetGroggy()
{
	bIsGroggy = true;
	// Groggy Widget 활성화
	GroggyWidget->SetVisibility(true);
	// 이동속도 0으로 설정
	GetCharacterMovement()->MaxWalkSpeed = 0.f;
}

float APlayerBase::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	Stat->ApplyDamage(DamageAmount);

	return 0.f;
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
	if (bIsGroggy)
		return;

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
	
	if (bIsCrouched)
	{
		CurrentNoise = 0.f;
	}
	else
	{
		if (bIsRunning)
		{
			CurrentNoise = RunNoise;
		}
		else
		{
			CurrentNoise = WalkNoise;
		}
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
	if (bIsGroggy) 
		return;

	bIsRunning = true;

	GetCharacterMovement()->MaxWalkSpeed = 800.f; // 기본 걷기보다 빠르게 설정
	if (!HasAuthority())
	{
		GetCharacterMovement()->MaxWalkSpeed = 800.f;
	}
	C2S_SetMaxWalkSpeed(800.f);
}

void APlayerBase::StopRun(const FInputActionValue& Value)
{
	if (bIsGroggy)
		return;

	bIsRunning = false;

	GetCharacterMovement()->MaxWalkSpeed = 200.f; // 걷기 속도로 복구
	if (!HasAuthority())
	{
		GetCharacterMovement()->MaxWalkSpeed = 200.f;
	}
	C2S_SetMaxWalkSpeed(200.f);

	CurrentNoise = 0.f;
}

void APlayerBase::PlayerCrouch(const FInputActionValue& Value)
{
	if (bIsGroggy)
		return;

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
	if (bIsGroggy)
		return;

	UE_LOG(LogTemp, Log, TEXT("Hacking Start"));

	if (NearestInteractiveObject->GetClass()->ImplementsInterface(UHacking::StaticClass()))
	{
		C2S_Hacking(NearestInteractiveObject);
	}

	CurrentNoise = 0.f;
}

void APlayerBase::StopHacking(const FInputActionValue& Value)
{
	if (bIsGroggy)
		return;

	UE_LOG(LogTemp, Log, TEXT("Hacking Stop"));
	 
	if (NearestInteractiveObject 
		&& NearestInteractiveObject->GetClass()->ImplementsInterface(UHacking::StaticClass()))
	{
		IHacking::Execute_OnHackingCompleted(NearestInteractiveObject);
	}
}

void APlayerBase::Interactive(const FInputActionValue& Value)
{
	if (bIsGroggy)
		return;

	if (NearestInteractiveObject 
		&& NearestInteractiveObject->GetClass()->ImplementsInterface(UInteraction::StaticClass()))
	{
		if (IInteraction::Execute_CanInteract(NearestInteractiveObject, this))
		{
			C2S_Interactive(NearestInteractiveObject);
			IInteraction::Execute_OnInteractClient(NearestInteractiveObject, this);
		}

		if (IInteraction::Execute_GetPickedUp(NearestInteractiveObject))
		{
			InventoryObjects.Add(NearestInteractiveObject);
			AddItemToUI();
		}
	}

	CurrentNoise = 0.f;
}

void APlayerBase::C2S_Interactive_Implementation(UObject* interact)
{
	if (nullptr == interact)
	{
		return;
	}

	if (interact->GetClass()->ImplementsInterface(UInteraction::StaticClass()))
		IInteraction::Execute_OnInteractSever(interact, this);
}

void APlayerBase::C2S_Hacking_Implementation(UObject* interact)
{
	UE_LOG(LogTemp, Warning, TEXT("A3333333333333333333333333333322222222222222222222222222I Is Hacked Task Executed"));
	if (nullptr == interact)
	{
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT("33333333333333333333333A22222222222222222222222222I Is Hacked Task Executed"));
	if (interact->GetClass()->ImplementsInterface(UHacking::StaticClass()))
		IHacking::Execute_OnHackingStarted(interact);
}

void APlayerBase::C2S_SetMaxWalkSpeed_Implementation(float Speed)
{
	GetCharacterMovement()->MaxWalkSpeed = Speed;
}

void APlayerBase::S2C_UpdatePerceivedActor_Implementation(AActor* Actor, bool bVisible)
{
	if (nullptr == Actor)
		return;

	// Add/Delete Object Array
	if (bVisible)
	{
		InteractiveableObjects.AddUnique(Actor);
	}
	else
	{
		InteractiveableObjects.Remove(Actor);

		// Set Object UI
		if (UWidgetComponent* Widget = Actor->FindComponentByClass<UWidgetComponent>())
		{
			UE_LOG(LogTemp, Log, TEXT("Widget Off"));
			Widget->SetVisibility(false);
		}
	}
}

void APlayerBase::OpenInventory(const FInputActionValue& Value)
{
	if (bIsGroggy)
		return;

	CurrentNoise = 0.f;

	APlayerController* PC = CastChecked<APlayerController>(GetController());

	if (!PC || !InvenWidget) return;

	bIsInventoryVisible = !bIsInventoryVisible;

	if (bIsInventoryVisible)
	{
		//UE_LOG(LogTemp, Log, TEXT("OpenInventory"));
		InvenWidget->SetVisibility(ESlateVisibility::Visible);

		FInputModeGameAndUI InputMode;
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		InputMode.SetHideCursorDuringCapture(false);
		PC->SetInputMode(InputMode);

		PC->bShowMouseCursor = true;
	}
	else
	{
		//UE_LOG(LogTemp, Log, TEXT("CloseInventory"));
		InvenWidget->SetVisibility(ESlateVisibility::Hidden);

		FInputModeGameOnly InputMode;
		PC->SetInputMode(InputMode);

		PC->bShowMouseCursor = false;
	}
}

