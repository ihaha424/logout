// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerCharacter.h"

#include "AbilitySystemComponent.h"
#include "PS_Player.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "New_ThePhantomTwins/Attribute/PlayerAttributeSet.h"
#include "New_ThePhantomTwins/Log/TPTLog.h"

// Sets default values
APlayerCharacter::APlayerCharacter()
{
    ASC = nullptr;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// movement setting
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0, 500, 0);
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 0.f;
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->bEnableCameraLag = true;
	SpringArm->TargetArmLength = 150.0f;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->bUsePawnControlRotation = false;
	Camera->SetupAttachment(SpringArm);
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void APlayerCharacter::PossessedBy(AController* NewController)
{
	// PossessedBy는 플레이어가 컨트롤러에 의해 소유될 때 호출된다. 즉, 빙의 될때 호출된다.
	// 그리고 이거는 서버에서만 호출되기 때문에 Onrep_PossessedBy를 이용해야한다. -> 3감에 있음
	Super::PossessedBy(NewController);
	PS = GetPlayerState<APS_Player>();

	if (PS)
	{
		ASC = PS->GetAbilitySystemComponent();
		ASC->InitAbilityActorInfo(PS, this);

		const UPlayerAttributeSet* CurrentAttributeSet = ASC->GetSet<UPlayerAttributeSet>();
		if (CurrentAttributeSet)
		{
			CurrentAttributeSet->OnPlayerConfused.AddDynamic(this, &ThisClass::OnPlayerDowned);
			CurrentAttributeSet->OnPlayerConfused.AddDynamic(this, &ThisClass::OnPlayerConfused);
		}
        if (InitAttributeSetEffect)
        {
            ASC->ApplyGameplayEffectToSelf(
                InitAttributeSetEffect->GetDefaultObject<UGameplayEffect>(),
                1.0f,
                ASC->MakeEffectContext()
            );
        }

		for (const auto& InputAbility : InputAbilities)
		{
			FGameplayAbilitySpec StartSpec(InputAbility.Value);
			StartSpec.InputID = InputAbility.Key;
			ASC->GiveAbility(StartSpec);
		}

		// 이렇게 하면 디버그 계속볼수있음.
		APlayerController* PlayerController = CastChecked<APlayerController>(NewController);
		PlayerController->ConsoleCommand(TEXT("showdebug abilitysystem"));
	}
}

void APlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	PS = GetPlayerState<APS_Player>();

	if (PS)
	{
		ASC = PS->GetAbilitySystemComponent();
		ASC->InitAbilityActorInfo(PS, this);

		const UPlayerAttributeSet* CurrentAttributeSet = ASC->GetSet<UPlayerAttributeSet>();
		if (CurrentAttributeSet)
		{
			CurrentAttributeSet->OnPlayerDowned.AddDynamic(this, &ThisClass::OnPlayerDowned);
			CurrentAttributeSet->OnPlayerConfused.AddDynamic(this, &ThisClass::OnPlayerConfused);
		}
		if (InitAttributeSetEffect)
		{
			ASC->ApplyGameplayEffectToSelf(
				InitAttributeSetEffect->GetDefaultObject<UGameplayEffect>(),
				1.0f,
				ASC->MakeEffectContext()
			);
		}
	}
}

class UAbilitySystemComponent* APlayerCharacter::GetAbilitySystemComponent() const
{
	return ASC;
}


void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);

	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Move);
	EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Look);
	EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Interact);

	SetupPlayerInputByTag();
}

void APlayerCharacter::SetupPlayerInputByTag()
{
	if (IsValid(ASC) && IsValid(InputComponent))
	{
		UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);
	
		EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Triggered, this, &APlayerCharacter::GASInputPressed, 0);
	}
}
void APlayerCharacter::OnPlayerDowned()
{
	// 기절 상태.
}

void APlayerCharacter::OnPlayerConfused()
{
	// 착란 상태.
}


void APlayerCharacter::Move(const FInputActionValue& Value)
{
	if (PS && Controller)
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

void APlayerCharacter::Look(const FInputActionValue& Value)
{
	if (Controller)
	{
		FVector2D LookAxisVector = Value.Get<FVector2D>();

		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void APlayerCharacter::Interact(const FInputActionValue& Value)
{
}

void APlayerCharacter::GASInputPressed(int32 InputId)
{
	FGameplayAbilitySpec* Spec = ASC->FindAbilitySpecFromInputID(InputId);
	if (Spec)
	{
		Spec->InputPressed = true;
		if (Spec->IsActive())
		{
			ASC->AbilitySpecInputPressed(*Spec);
		}
		else
		{
			ASC->TryActivateAbility(Spec->Handle);
		}
	}
}

void APlayerCharacter::GASInputReleased(int32 InputId)
{
	FGameplayAbilitySpec* Spec = ASC->FindAbilitySpecFromInputID(InputId);
	if (Spec)
	{
		Spec->InputPressed = false;
		if (Spec->IsActive())
		{
			ASC->AbilitySpecInputReleased(*Spec);
		}
	}
}