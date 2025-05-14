#include "Character/PlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Game/TestPlayerController.h"
#include "Components/WidgetComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISense_Hearing.h"
#include "Object/TestObject.h"
#include "DrawDebugHelpers.h"
#include "UObject\ConstructorHelpers.h"
#include "Engine/Engine.h"
#include "Engine\StaticMesh.h"

APlayerCharacter::APlayerCharacter()
{
	UE_LOG(LogTemp, Warning, TEXT("Asdasfs"));
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
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	// 음.. 여기 좀 어렵다 코드 뭔지 잘 모르겠음
	// IMC를 쓰는데 EnhancedInputLocalPlayerSubsystem을 쓰는 이유는??
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
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ATestPlayerController* PlayerController = Cast<ATestPlayerController>(GetController());

	if (!PlayerController || !PlayerController->Perception)
		return;

	// 현재 퍼셉션 컴포넌트에 인지된 오브젝트 전부 다 가져오기
	TArray<AActor*> PerceptionActors;
	PlayerController->Perception->GetCurrentlyPerceivedActors(UAISense_Sight::StaticClass(), PerceptionActors);

	FVector Start = Camera->GetComponentLocation();
	FVector Direction = Camera->GetForwardVector(); // 반드시 Normalize 되어 있어야 함

	//DrawDebugLine(GetWorld(), Start, Start + Direction * 100.f, FColor::Blue, false, 1.0f, 0, 2.0f);
	Direction.Normalize();

	// 가장 가까운 오브젝트를 찾기 위한 거리 변수
	// 매번 초기화를 꼭 해줘야 한다
	float MinDistance = PlayerController->SightConfig->SightRadius - 200.f;

	// 가장 가까운 오브젝트를 찾고 상태를 변경시키기 위한 로직
	for (AActor* Actor : PerceptionActors)
	{
		ATestObject* TestObject = Cast<ATestObject>(Actor);
		if (!TestObject) continue; // 아니면 넘김

		FVector ToPoint = TestObject->GetActorLocation() - Start;

		// 직선과 점 사이의 거리: |Dir x VecToPoint| / |Dir|
		float DistanceFromLine = FVector::CrossProduct(Direction, ToPoint).Size();

		if (DistanceFromLine < MinDistance)
		{
			// 가까운 오브젝트가 있을 때
			MinDistance = DistanceFromLine;
			NearestInteractiveActor = TestObject;
		}
	}

	// 위젯 초기화
	for (AActor* Actor : PerceptionActors)
	{
		if (ATestObject* Obj = Cast<ATestObject>(Actor))
		{
			Obj->Widget->SetWidgetClass(DefaultWidgetClass);
		}
	}

	if (NearestInteractiveActor)
	{
		NearestInteractiveActor->Widget->SetWidgetClass(FocusedWidgetClass);
	}

}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);
	
	// Move Action
	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Move);
	// Look Action
	EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Look);
	// Run Action
	EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Run);
	EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Completed, this, &APlayerCharacter::StopRun);
	// Crouch Action
	EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Triggered, this, &APlayerCharacter::PlayerCrouch);
	// Hacking Action
	EnhancedInputComponent->BindAction(HackingAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Hacking);
	// Interactive Action
	EnhancedInputComponent->BindAction(InteractiveAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Interactive);
}

void APlayerCharacter::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	AddMovementInput(ForwardDirection, MovementVector.Y);
	AddMovementInput(RightDirection, MovementVector.X);
}

void APlayerCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	AddControllerYawInput(LookAxisVector.X);
	AddControllerPitchInput(LookAxisVector.Y);
}

void APlayerCharacter::Run(const FInputActionValue& Value)
{
	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, TEXT("Run"));
	GetCharacterMovement()->MaxWalkSpeed = 800.f; // 기본 걷기보다 빠르게 설정
}

void APlayerCharacter::StopRun(const FInputActionValue& Value)
{
	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, TEXT("Walk"));
	GetCharacterMovement()->MaxWalkSpeed = 200.f; // 걷기 속도로 복구
}

void APlayerCharacter::PlayerCrouch(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Log, TEXT("Crouch"));
	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, TEXT("Crouch"));
	GetCharacterMovement()->MaxWalkSpeed = 100.f;
}

void APlayerCharacter::Hacking(const FInputActionValue& Value)
{
	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, TEXT("Hacking Start"));
	UE_LOG(LogTemp, Log, TEXT("Hacking Start"));
}

void APlayerCharacter::Interactive(const FInputActionValue& Value)
{
	if (NearestInteractiveActor && NearestInteractiveActor->Widget->IsVisible())
	{
		// 상호작용 된 오브젝트 메쉬 바꿔주기
		NearestInteractiveActor->Mesh->SetStaticMesh(InteractiveMesh);
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, TEXT("Interactive Object"));
		UE_LOG(LogTemp, Log, TEXT("Interactive Object"));
	}
}

void APlayerCharacter::ReferenceSetting()
{
	// Set the default input mapping context
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> InputMappingContextRef(TEXT("/PlayerPlugin/MyProject/Input/IMC_PlayerIMC.IMC_PlayerIMC"));
	if (InputMappingContextRef.Object)
	{
		InputMappingContext = InputMappingContextRef.Object;
	}

	// Set the default values for the input actions
	static ConstructorHelpers::FObjectFinder<UInputAction> MoveActionRef(TEXT("/PlayerPlugin/MyProject/Input/Actions/IA_Move.IA_Move"));
	if (MoveActionRef.Object)
	{
		MoveAction = MoveActionRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> LookActionRef(TEXT("/PlayerPlugin/MyProject/Input/Actions/IA_Look.IA_Look"));
	if (LookActionRef.Object)
	{
		LookAction = LookActionRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> RunActionRef(TEXT("/PlayerPlugin/MyProject/Input/Actions/IA_Run.IA_Run"));
	if (RunActionRef.Object)
	{
		RunAction = RunActionRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> CrouchActionRef(TEXT("/PlayerPlugin/MyProject/Input/Actions/IA_Crouch.IA_Crouch"));
	if (CrouchActionRef.Object)
	{
		CrouchAction = CrouchActionRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> HackingActionRef(TEXT("/PlayerPlugin/MyProject/Input/Actions/IA_Hacking.IA_Hacking"));
	if (HackingActionRef.Object)
	{
		HackingAction = HackingActionRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> InteractiveActionRef(TEXT("/PlayerPlugin/MyProject/Input/Actions/IA_Interactive.IA_Interactive"));
	if (InteractiveActionRef.Object)
	{
		InteractiveAction = InteractiveActionRef.Object;
	}

	static ConstructorHelpers::FClassFinder<UUserWidget> DefaultWidgetRef(TEXT("/PlayerPlugin/MyProject/Blueprints/UI/NewWidgetBlueprint.NewWidgetBlueprint_C"));
	if (DefaultWidgetRef.Class)
	{
		DefaultWidgetClass = DefaultWidgetRef.Class;
	}

	static ConstructorHelpers::FClassFinder<UUserWidget> FocusedWidgetRef(TEXT("/PlayerPlugin/MyProject/Blueprints/UI/WB_FButton.WB_FButton_C"));
	if (FocusedWidgetRef.Class)
	{
		FocusedWidgetClass = FocusedWidgetRef.Class;
	}

	static ConstructorHelpers::FObjectFinder<UStaticMesh> InteractiveObjectMeshRef(TEXT("/PlayerPlugin/LevelPrototyping/Meshes/SM_Cube.SM_Cube"));
	if (InteractiveObjectMeshRef.Object)
	{
		InteractiveMesh = InteractiveObjectMeshRef.Object;
	}
}