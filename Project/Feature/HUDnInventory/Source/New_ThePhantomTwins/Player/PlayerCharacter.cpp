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

		ASC->AddLooseGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_Identifier_Player);
	}
	PlayerController = GetController<APC_Player>();
	NULLCHECK_RETURN_LOG(PlayerController, PlayerLog, Error, );
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
	NULLCHECK_RETURN_LOG(PS, PlayerLog, Error, );

	ASC = PS->GetAbilitySystemComponent();
	ASC->InitAbilityActorInfo(PS, this);
	NULLCHECK_RETURN_LOG(ASC, PlayerLog, Error, );
	const UPlayerAttributeSet* AttributeSet = ASC->GetSet<UPlayerAttributeSet>();
	NULLCHECK_RETURN_LOG(AttributeSet, PlayerLog, Error, );
	BindAttributeDelegates(AttributeSet);

	NULLCHECK_RETURN_LOG(InteractWidget, PlayerLog, Error, );
	UKismetSystemLibrary::PrintString(this, FString("ddsaf"));
	//InteractWidget->SetWidgetClass(InteractWidgetClass);
	InteractWidget->SetVisibility(false);

	NULLCHECK_RETURN_LOG(PlayerController, PlayerLog, Error, );
	PlayerController->RegisterWidget(TEXT("RecoveryGauge"), CreateWidget<UUserWidget>(GetWorld(), RecoveryWidgetClass));
}


void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//NULLCHECK_RETURN_LOG(PlayerController, PlayerLog, Error, );
	//NULLCHECK_RETURN_LOG(FocusTrace, PlayerLog, Error, );
	if (PlayerController&& FocusTrace)
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
		TPTInput->BindActionByTag(InputConfig, FTPTGameplayTags::Get().TPTGameplay_InputTag_Player_ItemSlot_1st, ETriggerEvent::Started, this, &ThisClass::InputPressedWithNum,1);
		TPTInput->BindActionByTag(InputConfig, FTPTGameplayTags::Get().TPTGameplay_InputTag_Player_ItemSlot_2nd, ETriggerEvent::Started, this, &ThisClass::InputPressedWithNum,2);
		TPTInput->BindActionByTag(InputConfig, FTPTGameplayTags::Get().TPTGameplay_InputTag_Player_ItemSlot_3rd, ETriggerEvent::Started, this, &ThisClass::InputPressedWithNum,3);
		TPTInput->BindActionByTag(InputConfig, FTPTGameplayTags::Get().TPTGameplay_InputTag_Player_ItemSlot_4th, ETriggerEvent::Started, this, &ThisClass::InputPressedWithNum,4);
		TPTInput->BindActionByTag(InputConfig, FTPTGameplayTags::Get().TPTGameplay_InputTag_Player_ItemSlot_5th, ETriggerEvent::Started, this, &ThisClass::InputPressedWithNum,5);
	}
}
void APlayerCharacter::InputPressed(int32 InputID)
{
	FGameplayAbilitySpec* Spec = ASC->FindAbilitySpecFromInputID(InputID);
	if (Spec)
	{
		//Spec->GameplayEventData
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

void APlayerCharacter::InputPressedWithNum(int32 InputID, int32 Number)
{
	FGameplayTag EventTag = FTPTGameplayTags::Get().TPTGameplay_Event_Character_UseItemSlot;
	FGameplayEventData Payload;
	Payload.EventTag = EventTag;
	Payload.Instigator = this;	// 이벤트를 유발한 주체 
	Payload.EventMagnitude = static_cast<float>(Number);
	//TPT_LOG(PlayerLog, Log, TEXT("슬롯 번호: %f"), Payload.EventMagnitude);

	ASC->HandleGameplayEvent(EventTag, &Payload);
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
	NULLCHECK_RETURN_LOG(PS, PlayerLog, Error, );
	NULLCHECK_RETURN_LOG(ASC, PlayerLog, Error, );
	PS->SetRecovery(true);

	FGameplayTag DownedTag = FTPTGameplayTags::Get().TPTGameplay_Character_State_Downed;

	int32 Count = ASC->GetTagCount(DownedTag);
	for (int32 i = 0; i < Count; ++i)
	{
		ASC->RemoveLooseGameplayTag(DownedTag);
	}

	GetWorld()->GetTimerManager().ClearTimer(RecoveryTimerHandle);

	// TODO : 회복 GE
	UKismetSystemLibrary::PrintString(this, FString("Recovery"));
	TPT_LOG(PlayerLog, Log, TEXT("Recovery %s"), *this->GetFName().ToString());
}

bool APlayerCharacter::CanInteract_Implementation(const APawn* Interactor, bool bIsDetected)
{
	NULLCHECK_RETURN_LOG(ASC, PlayerLog, Error, false);
	if (!Interactor->IsLocallyControlled()) return false;

	if (bIsDetected)
	{
		bool bIsTag = ASC->HasMatchingGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_State_Downed);
		if (bIsTag)
		{
			if (InteractWidget)
			{
				//UKismetSystemLibrary::PrintString(this, FString("True"));
				InteractWidget->SetVisibility(true);
			}
			return true;
		}
	}
	
	if (InteractWidget)
	{
		InteractWidget->SetVisibility(false);
		return false;
	}

	return false;
}

void APlayerCharacter::OnInteractServer_Implementation(const APawn* Interactor)
{
	UKismetSystemLibrary::PrintString(this, FString("Downed Character"));
	UKismetSystemLibrary::PrintString(Interactor, FString("Interact Character"));
	TPT_LOG(PlayerLog, Log, TEXT("Downed PS : %s"), *this->PS.GetFName().ToString());

	GetWorld()->GetTimerManager().SetTimer(
		RecoveryTimerHandle,               
		this,                              
		&APlayerCharacter::OnRecoveryCompelete, 
		2.0f,                              
		false                              
	);
}

void APlayerCharacter::OnInteractClient_Implementation(const APawn* Interactor)
{
	NULLCHECK_RETURN_LOG(PlayerController, PlayerLog, Error, );
	PlayerController->SetWidget(TEXT("RecoveryGauge"), true, EMessageTargetType::Multicast);
}

void APlayerCharacter::ExecuteAbilityByTag(FGameplayTag InputTag)
{
	FGameplayAbilitySpec* TagID = ASC->FindAbilitySpecFromInputID(static_cast<int32>(FTPTGameplayTags::Get().TagMap[InputTag]));
	NULLCHECK_RETURN_LOG(TagID, PlayerLog, Error, );
	bool CanActivate = ASC->TryActivateAbilitiesByTag(FGameplayTagContainer(InputTag));
	TPT_LOG(PlayerLog, Log, TEXT("Tag :::  %s / Activate Ability success ? ::: %d / Ability IsValid ? ::: %d"), *InputTag.ToString(), CanActivate, IsValid(TagID->Ability));

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