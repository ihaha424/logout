// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerCharacter.h"

#include "AbilitySystemComponent.h"
#include "PS_Player.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Attribute/PlayerAttributeSet.h"
#include "Input/TPTEnhancedInputComponent.h"
#include "Log/TPTLog.h"
#include "Tags/TPTGameplayTags.h"
#include "FocusTraceComponent.h"
#include "../GA/Action/GA_Interact.h"

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
	GetCharacterMovement()->MaxWalkSpeedCrouched = 80.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->bEnableCameraLag = true;
	SpringArm->TargetArmLength = 150.0f;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->bUsePawnControlRotation = false;
	Camera->SetupAttachment(SpringArm);

	FocusTrace = CreateDefaultSubobject<UFocusTraceComponent>(TEXT("FocusTrace"));
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

		const UPlayerAttributeSet* AttributeSet = ASC->GetSet<UPlayerAttributeSet>();
		if (AttributeSet)
		{
			BindAttributeDelegates(AttributeSet);
		}
		if (InitAttributeSetEffect)
		{
			ASC->ApplyGameplayEffectToSelf
			(
				InitAttributeSetEffect->GetDefaultObject<UGameplayEffect>(),
				1.0f,
				ASC->MakeEffectContext()
			);
		}
		for (const auto& Ability : PlayerAbilities)
		{
			const EFTPTGameplayTags* TagEnum = FTPTGameplayTags::Get().TagMap.Find(Ability.Key);
			int32 InputID = static_cast<int32>(*TagEnum);

			FGameplayAbilitySpec StartSpec(Ability.Value);
			StartSpec.InputID = InputID;
			ASC->GiveAbility(StartSpec);
		}

		APlayerController* PlayerController = CastChecked<APlayerController>(GetController());
		PlayerController->ConsoleCommand(TEXT("showdebug abilitysystem"));
	}
}

void APlayerCharacter::OnRep_Controller()
{
	Super::OnRep_Controller();
	if (!HasAuthority())
	{
		APlayerController* PlayerController = CastChecked<APlayerController>(GetController());
		//PlayerController->ConsoleCommand(TEXT("showdebug abilitysystem"));
		if (FocusTrace)
		{
			FVector2D ViewportSize;
			GEngine->GameViewport->GetViewportSize(ViewportSize);

			FVector WorldLocation;
			FVector WorldDirection;

			PlayerController->DeprojectScreenPositionToWorld(ViewportSize.X * 0.5f, ViewportSize.Y * 0.5f, WorldLocation, WorldDirection);

			FocusTrace->SetStart(WorldLocation);
			FocusTrace->SetDirection(WorldDirection);
		}
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

		const UPlayerAttributeSet* AttributeSet = ASC->GetSet<UPlayerAttributeSet>();
		if (AttributeSet)
		{
			BindAttributeDelegates(AttributeSet);
		}
	}
}


void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	UTPTEnhancedInputComponent* TPTInput = CastChecked<UTPTEnhancedInputComponent>(PlayerInputComponent);
	check(TPTInput);
	TPTInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ThisClass::Move);
	TPTInput->BindAction(LookAction, ETriggerEvent::Triggered, this, &ThisClass::Look);

	SetupPlayerInputByTag(TPTInput);
}

void APlayerCharacter::SetupPlayerInputByTag(UTPTEnhancedInputComponent* TPTInput)
{
	if (IsValid(ASC) && IsValid(TPTInput))
	{
		TPTInput->BindActionByTag(InputConfig, FTPTGameplayTags::Get().TPTGameplay_InputTag_Player_Run, ETriggerEvent::Triggered, this, &ThisClass::InputPressed);
		TPTInput->BindActionByTag(InputConfig, FTPTGameplayTags::Get().TPTGameplay_InputTag_Player_Run, ETriggerEvent::Completed, this, &ThisClass::InputReleased);
		TPTInput->BindActionByTag(InputConfig, FTPTGameplayTags::Get().TPTGameplay_InputTag_Player_Crouch, ETriggerEvent::Triggered, this, &ThisClass::InputPressed);

		TPTInput->BindActionByTag(InputConfig, FTPTGameplayTags::Get().TPTGameplay_InputTag_Player_Interact, ETriggerEvent::Triggered, this, &ThisClass::InputPressed);
		TPTInput->BindActionByTag(InputConfig, FTPTGameplayTags::Get().TPTGameplay_InputTag_Player_LookBack, ETriggerEvent::Started, this, &ThisClass::InputPressed);
		TPTInput->BindActionByTag(InputConfig, FTPTGameplayTags::Get().TPTGameplay_InputTag_Player_LookBack, ETriggerEvent::Completed, this, &ThisClass::InputReleased);

		TPTInput->BindActionByTag(InputConfig, FTPTGameplayTags::Get().TPTGameplay_InputTag_Player_ActiveSkill, ETriggerEvent::Started, this, &ThisClass::InputPressed);
		TPTInput->BindActionByTag(InputConfig, FTPTGameplayTags::Get().TPTGameplay_InputTag_Player_ItemSlot_1st, ETriggerEvent::Triggered, this, &ThisClass::InputPressed);
		TPTInput->BindActionByTag(InputConfig, FTPTGameplayTags::Get().TPTGameplay_InputTag_Player_ItemSlot_2nd, ETriggerEvent::Triggered, this, &ThisClass::InputPressed);
		TPTInput->BindActionByTag(InputConfig, FTPTGameplayTags::Get().TPTGameplay_InputTag_Player_ItemSlot_3rd, ETriggerEvent::Triggered, this, &ThisClass::InputPressed);
		TPTInput->BindActionByTag(InputConfig, FTPTGameplayTags::Get().TPTGameplay_InputTag_Player_ItemSlot_4th, ETriggerEvent::Triggered, this, &ThisClass::InputPressed);
		TPTInput->BindActionByTag(InputConfig, FTPTGameplayTags::Get().TPTGameplay_InputTag_Player_ItemSlot_5th, ETriggerEvent::Triggered, this, &ThisClass::InputPressed);
	}
}

void APlayerCharacter::ExecuteAbilityByTag(FGameplayTag InputTag)
{
	FGameplayAbilitySpec* temp = ASC->FindAbilitySpecFromInputID(static_cast<int32>(FTPTGameplayTags::Get().TagMap[InputTag]));
	
	bool bo = ASC->TryActivateAbilitiesByTag(FGameplayTagContainer(InputTag));
	TPT_LOG(GALog, Error, TEXT("%s   ::: %d ::: %d"),*InputTag.ToString(), bo, IsValid(temp->Ability));
}

void APlayerCharacter::BindAttributeDelegates(const UPlayerAttributeSet* AttributeSet)
{
	TPT_LOG(GALog, Error, TEXT("3"));
	AttributeSet->OnPlayerDowned.AddDynamic(this, &ThisClass::ExecuteAbilityByTag);
	AttributeSet->OnPlayerConfused1st.AddDynamic(this, &ThisClass::ExecuteAbilityByTag);
	AttributeSet->OnPlayerConfused2nd.AddDynamic(this, &ThisClass::ExecuteAbilityByTag);
	AttributeSet->OnPlayerConfused3rd.AddDynamic(this, &ThisClass::ExecuteAbilityByTag);
	AttributeSet->OnPlayerUseSkill.AddDynamic(this, &ThisClass::ExecuteAbilityByTag);
}

void APlayerCharacter::InputPressed(int32 InputID)
{
	FGameplayAbilitySpec* Spec = ASC->FindAbilitySpecFromInputID(InputID);
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

void APlayerCharacter::InputReleased(int32 InputID)
{
	FGameplayAbilitySpec* Spec = ASC->FindAbilitySpecFromInputID(InputID);
	Spec->InputPressed = false;
	if (Spec->IsActive())
	{
		ASC->AbilitySpecInputReleased(*Spec);
	}
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

UAbilitySystemComponent* APlayerCharacter::GetAbilitySystemComponent() const
{
	return ASC;
}

// AI의 감지를 위한 팀설정.
FGenericTeamId APlayerCharacter::GetGenericTeamId() const
{
	if (PS)
	{
		return PS->GetGenericTeamId();
	}
	return FGenericTeamId::NoTeam;
}