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
#include "PC_Player.h"
#include "../GA/Action/GA_Interact.h"
#include "UI/HUD/HUD_PhantomTwins.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "UIManager/UIManager.h"
#include "Components/WidgetComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UIManager/UIManager.h"

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

	InteractWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractWidget"));
	InteractWidget->SetupAttachment(GetMesh());
	InteractWidget->SetWidgetSpace(EWidgetSpace::Screen);
	InteractWidget->SetDrawSize(FVector2D(10, 10));
	InteractWidget->SetRelativeLocation(FVector(0, 0, 100));
	InteractWidget->SetVisibility(false);

	bReplicates = true;
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void APlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	PS = GetPlayerState<APS_Player>();
	NULLCHECK_RETURN_LOG(PS, PlayerLog, Error, );
	
	ASC = PS->GetAbilitySystemComponent();
	NULLCHECK_RETURN_LOG(ASC, PlayerLog, Error, );

	ASC->InitAbilityActorInfo(PS, this);
	const UPlayerAttributeSet* AttributeSet = ASC->GetSet<UPlayerAttributeSet>();
	NULLCHECK_RETURN_LOG(AttributeSet, PlayerLog, Error, );

	BindAttributeDelegates(AttributeSet);
	NULLCHECK_RETURN_LOG(InitAttributeSetEffect, PlayerLog, Error, );
	ASC->ApplyGameplayEffectToSelf
	(
		InitAttributeSetEffect->GetDefaultObject<UGameplayEffect>(),
		1.0f,
		ASC->MakeEffectContext()
	);
	
	for (const auto& Ability : PlayerAbilities)
	{
		const EFTPTGameplayTags* TagEnum = FTPTGameplayTags::Get().TagMap.Find(Ability.Key);
		int32 InputID = static_cast<int32>(*TagEnum);

		FGameplayAbilitySpec StartSpec(Ability.Value);
		StartSpec.InputID = InputID;
		ASC->GiveAbility(StartSpec);
	}
	PlayerController = GetController<APC_Player>();
	NULLCHECK_RETURN_LOG(PlayerController, PlayerLog, Error, );

	PlayerController->RegisterWidget(TEXT("RecoveryGauge"), CreateWidget<UUserWidget>(GetWorld(), RecoveryWidgetClass));
	//RecoveryWidget->SetWidgetClass(RecoveryWidgetClass);

	if (InteractWidget)
	{
		InteractWidget->SetWidgetClass(InteractWidgetClass);
	}
}

void APlayerCharacter::OnRep_Controller()
{
	Super::OnRep_Controller();
	if (!HasAuthority())
	{
		PlayerController = CastChecked<APC_Player>(GetController());
		NULLCHECK_RETURN_LOG(PlayerController, PlayerLog, Error, );
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
		NULLCHECK_RETURN_LOG(ASC, PlayerLog, Error, );
		const UPlayerAttributeSet* AttributeSet = ASC->GetSet<UPlayerAttributeSet>();
		NULLCHECK_RETURN_LOG(AttributeSet, PlayerLog, Error, );
		BindAttributeDelegates(AttributeSet);
	}
}


void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (FocusTrace && PlayerController)
	{
		FVector2D ViewportSize;
		GEngine->GameViewport->GetViewportSize(ViewportSize);

		FVector WorldLocation;
		FVector WorldDirection;

		PlayerController->DeprojectScreenPositionToWorld(ViewportSize.X * 0.5f, ViewportSize.Y * 0.5f, WorldLocation, WorldDirection);

		FocusTrace->SetStart(WorldLocation);
		FocusTrace->SetDirection(WorldDirection);
		FocusTrace->SetCollisionType(ECC_WorldDynamic);
	}
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
		TPTInput->BindActionByTag(InputConfig, FTPTGameplayTags::Get().TPTGameplay_InputTag_Player_Run, ETriggerEvent::Started, this, &ThisClass::InputPressed);
		TPTInput->BindActionByTag(InputConfig, FTPTGameplayTags::Get().TPTGameplay_InputTag_Player_Run, ETriggerEvent::Completed, this, &ThisClass::InputReleased);
		TPTInput->BindActionByTag(InputConfig, FTPTGameplayTags::Get().TPTGameplay_InputTag_Player_Crouch, ETriggerEvent::Triggered, this, &ThisClass::InputPressed);

		TPTInput->BindActionByTag(InputConfig, FTPTGameplayTags::Get().TPTGameplay_InputTag_Player_Interact, ETriggerEvent::Started, this, &ThisClass::InputPressed);
		TPTInput->BindActionByTag(InputConfig, FTPTGameplayTags::Get().TPTGameplay_InputTag_Player_Interact, ETriggerEvent::Completed, this, &ThisClass::InputReleased);
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
void APlayerCharacter::InputPressed(int32 InputID)
{
	FGameplayAbilitySpec* Spec = ASC->FindAbilitySpecFromInputID(InputID);
	if (Spec)
	{
		Spec->InputPressed = true;
		if (Spec->IsActive())
		{
			//TPT_LOG(GALog, Log, TEXT("AbilitySpecInputPressed"));
			ASC->AbilitySpecInputPressed(*Spec);
		}
		else
		{
			//TPT_LOG(GALog, Log, TEXT("TryActivateAbility"));
			ASC->TryActivateAbility(Spec->Handle);
		}
	}
}
void APlayerCharacter::InputPressedWithNum(int32 InputID)
{
	FGameplayAbilitySpec* Spec = ASC->FindAbilitySpecFromInputID(InputID);
	if (Spec)
	{
		Spec->InputPressed = true;
		if (Spec->IsActive())
		{
			//TPT_LOG(GALog, Log, TEXT("AbilitySpecInputPressed"));
			ASC->AbilitySpecInputPressed(*Spec);
		}
		else
		{
			//TPT_LOG(GALog, Log, TEXT("TryActivateAbility"));
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

void APlayerCharacter::BindAttributeDelegates(const UPlayerAttributeSet* AttributeSet)
{
	//TPT_LOG(GALog, Error, TEXT("3"));
	AttributeSet->OnPlayerLowHP.AddDynamic(this, &ThisClass::ExecuteAbilityByTag);
	AttributeSet->OnPlayerDowned.AddDynamic(this, &ThisClass::ExecuteAbilityByTag);
	AttributeSet->OnPlayerConfused1st.AddDynamic(this, &ThisClass::ExecuteAbilityByTag);
	AttributeSet->OnPlayerConfused2nd.AddDynamic(this, &ThisClass::ExecuteAbilityByTag);
	AttributeSet->OnPlayerConfused3rd.AddDynamic(this, &ThisClass::ExecuteAbilityByTag);
	AttributeSet->OnPlayerUseSkill.AddDynamic(this, &ThisClass::ExecuteAbilityByTag);

	//AttributeSet->OnChangedHP.AddDynamic(this, &ThisClass::PlayerHUDHPSet);
	//AttributeSet->OnChangedMentalPoint.AddDynamic(this, &ThisClass::PlayerHUDMentalSet);
	//AttributeSet->OnChangedStamina.AddDynamic(this, &ThisClass::PlayerHUDStaminaSet);
	//AttributeSet->OnChangedCoreEnergy.AddDynamic(this, &ThisClass::PlayerHUDCoreEnergySet);
}

void APlayerCharacter::OnRecoveryCompelete()
{
	bIsRecovery = true;
}

bool APlayerCharacter::CanInteract_Implementation(const APawn* Interactor, bool bIsDetected)
{
	if (ASC == nullptr) return false;
	if (!Interactor->IsLocallyControlled()) return false;

	bool bIsTag = ASC->HasMatchingGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_State_Downed);
	if (bIsDetected && bIsTag)
	{
		if (InteractWidget)
		{
		TPT_LOG(LogTemp, Error, TEXT("%f, %f, %f"), InteractWidget->GetComponentToWorld().GetLocation().X, InteractWidget->GetComponentToWorld().GetLocation().Y, InteractWidget->GetComponentToWorld().GetLocation().Z);
			InteractWidget->SetVisibility(true);
		}
		return true;
	}
	else
	{
		if (InteractWidget)
		{
			InteractWidget->SetVisibility(false);
		}
		return false;
	}
}

void APlayerCharacter::OnInteractServer_Implementation(const APawn* Interactor)
{
	TPT_LOG(LogTemp, Log, TEXT("Player Interact"));

	GetWorld()->GetTimerManager().SetTimer(
		RecoveryTimerHandle,               // �ڵ�
		this,                              // ȣ�� ���
		&APlayerCharacter::OnRecoveryCompelete, // ������ �Լ�
		5.0f,                              // ������ �ð� (��)
		false                              // �ݺ� ���� (false = 1ȸ ����)
	);

	if (bIsRecovery)
	{
		// �� �±� ���� �� ��Ŀ���� �Ϸ� �±� �ֱ�
		ASC->RemoveLooseGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_State_Downed);
		ASC->AddLooseGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_State_Recovery);

		GetWorld()->GetTimerManager().ClearTimer(RecoveryTimerHandle);

		// TODO: 회복GE
	}
}

void APlayerCharacter::OnInteractClient_Implementation(const APawn* Interactor)
{
	if (APC_Player* PC = Interactor->GetController<APC_Player>())
	{
		PC->SetWidget(TEXT("RecoveryGauge"), true, EMessageTargetType::Multicast);
	}
}

void APlayerCharacter::ExecuteAbilityByTag(FGameplayTag InputTag)
{
	FGameplayAbilitySpec* TagID = ASC->FindAbilitySpecFromInputID(static_cast<int32>(FTPTGameplayTags::Get().TagMap[InputTag]));
	NULLCHECK_RETURN_LOG(TagID, PlayerLog, Error, );
	bool CanActivate = ASC->TryActivateAbilitiesByTag(FGameplayTagContainer(InputTag));
	TPT_LOG(PlayerLog, Log, TEXT("Tag  :::  %s / Activate success ? ::: %d / IsValid ? ::: %d"), *InputTag.ToString(), CanActivate, IsValid(TagID->Ability));

	// TODO : HandleGameplayEvent // 
}

void APlayerCharacter::PlayerHUDHPSet(int32 value)
{
	PlayerHUD->UpdateHP(value);

	//PlayerHUD->UpdateClearItem();// 데이터조각(같이공유하는거)
	//// 태그이용
	//PlayerHUD->SetActiveSkillIcon();
	//PlayerHUD->SetPassiveSkillIcon();
	//PlayerHUD->SetCharPortrait();// 초상화
}
void APlayerCharacter::PlayerHUDMentalSet(int32 value)
{
	PlayerHUD->UpdateMental(value);
}
void APlayerCharacter::PlayerHUDStaminaSet(int32 value)
{
	PlayerHUD->UpdateStamina(value);
}
void APlayerCharacter::PlayerHUDCoreEnergySet(int32 value)
{
	PlayerHUD->UpdateCoreEnergy(value);
}

void APlayerCharacter::Move(const FInputActionValue& Value)
{
	NULLCHECK_RETURN_LOG(PS, PlayerLog, Error, );
	NULLCHECK_RETURN_LOG(PlayerController, PlayerLog, Error, );

	FVector2D MovementVector = Value.Get<FVector2D>();

	const FRotator Rotation = PlayerController->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	AddMovementInput(ForwardDirection, MovementVector.Y);
	AddMovementInput(RightDirection, MovementVector.X);
}

void APlayerCharacter::Look(const FInputActionValue& Value)
{
	NULLCHECK_RETURN_LOG(PlayerController, PlayerLog, Error, );
	
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	AddControllerYawInput(LookAxisVector.X);
	AddControllerPitchInput(LookAxisVector.Y);
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