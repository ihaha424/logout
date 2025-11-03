// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerCharacter.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
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
#include "GameplayEffect.h"
#include "FocusTraceComponent.h"
#include "GM_PhantomTwins.h"
#include "PC_Player.h"
#include "PlayerAnimInstance.h"
#include "../GA/Action/GA_Interact.h"
#include "AI/Character/AIBaseCharacter.h"
#include "Blueprint/UserWidget.h"
#include "Components/AudioComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UIManager/UIManager.h"
#include "Components/WidgetComponent.h"
#include "Objects/InventoryComponent.h"
#include "Objects/HeldItemComponent.h"
#include "UI/HUD/PlayerHUDWidget.h"
#include "Net/UnrealNetwork.h"
#include "Components/PostProcessComponent.h"
#include "Components/BoxComponent.h"
#include "Data/DT_Skill.h"
#include "SaveGame/TPTSaveGameManager.h"
#include "UI/DroneStatWidget.h"

APlayerCharacter::APlayerCharacter()
{
	ASC = nullptr;
	bReplicates = true;
	SetReplicates(true);

	MovementSetting();
	SpeedSetting(WalkSpeed);
	CameraSetting();
	OverlapRangeSetting();

	FocusTrace = CreateDefaultSubobject<UFocusTraceComponent>(TEXT("FocusTrace"));
	FocusTrace->SetIsReplicated(true);

	InteractWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractWidget"));
	InteractWidget->SetupAttachment(GetMesh());

	DownedWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("DownedWidget"));
	DownedWidget->SetupAttachment(GetMesh());
	DownedWidget->SetRelativeLocation(FVector(0, 0, 0));

	DroneWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("DroneWidget"));
	DroneWidget->SetupAttachment(GetMesh());
	DroneWidget->SetRelativeLocation(FVector(0, 0, 0));

	PostProcessComp = CreateDefaultSubobject<UPostProcessComponent>(TEXT("PlayerVFX"));
	PostProcessComp->SetupAttachment(RootComponent);
	PostProcessComp->bUnbound = false;
	PostProcessComp->Priority = 1.f;

	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxComp->SetupAttachment(RootComponent);
	BoxComp->SetCollisionProfileName(TEXT("OverlapAll"));

	HeldItemComponent = CreateDefaultSubobject<UHeldItemComponent>(TEXT("HeldItemComponent"));
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

UAbilitySystemComponent* APlayerCharacter::GetAbilitySystemComponent() const
{
	return ASC;
}

void APlayerCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APlayerCharacter, RecoveryPercent);
	DOREPLIFETIME(APlayerCharacter, CurrentWallRange);
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	NULLCHECK_RETURN_LOG(InteractWidget, PlayerLog, Error, );
	NULLCHECK_RETURN_LOG(DownedWidget, PlayerLog, Error, );
	NULLCHECK_RETURN_LOG(InteractWidgetClass, PlayerLog, Error, );
	NULLCHECK_RETURN_LOG(DownWidgetClass, PlayerLog, Error, );

	UUserWidget* Interact = CreateWidget(GetWorld(), InteractWidgetClass);
	InteractWidget->SetWidget(Interact);
	InteractWidget->GetUserWidgetObject()->SetVisibility(ESlateVisibility::Hidden);

	UUserWidget* Down = CreateWidget(GetWorld(), DownWidgetClass);
	DownedWidget->SetWidget(Down);
	DownedWidget->GetUserWidgetObject()->SetVisibility(ESlateVisibility::Hidden);

	FocusTrace->SetIsReplicated(true);

	InitPostProcessComponent();

	// RecoveryGauge Time
	Time = RecoveryTime;
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsLocallyControlled() && PlayerController && FocusTrace)
	{
		FVector2D ViewportSize;
		GEngine->GameViewport->GetViewportSize(ViewportSize);

		FVector WorldLocation;
		FVector WorldDirection;
		PlayerController->DeprojectScreenPositionToWorld(ViewportSize.X * 0.5f, ViewportSize.Y * 0.5f, WorldLocation, WorldDirection);

		// 클라 내에서 카메라 위치와 회전 받아오기
		FVector CameraLocation;
		FRotator CameraRotation;
		PlayerController->GetPlayerViewPoint(CameraLocation, CameraRotation);
		
		// 위젯 위치와 방향 계산
		FVector WidgetLoc = DroneWidget->GetComponentLocation();

		// 카메라 → 위젯 방향 (즉, 서로 마주보게)
		FVector ToCamera = CameraLocation - WidgetLoc;
		ToCamera.Normalize();

		// 이 벡터를 "위젯이 바라볼 방향"으로 사용
		FRotator LookAtRot = ToCamera.Rotation();

		// Yaw만 회전하고 싶다면 Pitch, Roll 고정
		//LookAtRot.Pitch = 0.f;
		//LookAtRot.Roll = 0.f;

		// 회전 적용
		DroneWidget->SetWorldRotation(LookAtRot);
		ApplyDroneWidgetRotation(LookAtRot);

		//TPT_LOG(PlayerLog, Log, TEXT("%.2f"), -DroneWidget->GetRelativeRotation().Yaw);

		// 클라에선 FocusTrace 세팅(시각효과용)
		FocusTrace->SetStart(WorldLocation);
		FocusTrace->SetDirection(WorldDirection);

		// 서버 RPC 호출에 위치 + 회전 같이 넘기기
		C2S_SetFocusTrace(CameraLocation, CameraRotation);
	}
}

void APlayerCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorldTimerManager().ClearTimer(VisibleStaminaTimerHandle);
	GetWorldTimerManager().ClearTimer(VisibleInventoryTimerHandle);
	Super::EndPlay(EndPlayReason);
}

void APlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	PS = GetPlayerState<APS_Player>();
	NULLCHECK_RETURN_LOG(PS, PlayerLog, Error, );
	PS->SetIdentifyCharacterData();
	TPT_LOG(PlayerLog, Log, TEXT("Server Skill : %d, Client Skill : %d"), (int32)PS->IdentifyCharacterData.HostSkill, (int32)PS->IdentifyCharacterData.ClientSkill);
	SetMeshByCharacterType(PS);

	ASC = PS->GetAbilitySystemComponent();
	NULLCHECK_RETURN_LOG(ASC, PlayerLog, Error, );
	ASC->InitAbilityActorInfo(PS, this);

	const UPlayerAttributeSet* AttributeSet = ASC->GetSet<UPlayerAttributeSet>();
	NULLCHECK_RETURN_LOG(AttributeSet, PlayerLog, Error, );

	BindAttributeDelegates(AttributeSet);
	NULLCHECK_RETURN_LOG(InitAttributeSetEffect, PlayerLog, Error, );
	ASC->ApplyGameplayEffectToSelf(InitAttributeSetEffect->GetDefaultObject<UGameplayEffect>(), 1.0f, ASC->MakeEffectContext());

	for (const auto& Ability : PlayerAbilities)
	{
		const EFTPTGameplayTags* TagEnum = FTPTGameplayTags::Get().TagMap.Find(Ability.Key);
		int32 InputID = static_cast<int32>(*TagEnum);

		FGameplayAbilitySpec StartSpec(Ability.Value);
		StartSpec.InputID = InputID;
		ASC->GiveAbility(StartSpec);
	}

	ASC->AddLooseGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_Identifier_Player);
	PlayerController = GetController<APC_Player>();
	NULLCHECK_RETURN_LOG(PlayerController, PlayerLog, Error, );

	InitHUDWidget(AttributeSet);
	UPlayerAnimInstance* AnimInstance = Cast<UPlayerAnimInstance>(GetMesh()->GetAnimInstance());
	AnimInstance->InitializeWithAbilitySystem(ASC);

	InitPostProcessComponent();
	UTPTSaveGameManager* SaveGameManager = GetGameInstance()->GetSubsystem<UTPTSaveGameManager>();
	SaveGameManager->ApplyAuthorityPlayerSaveGame(PlayerController);

	EnsureSetting(EnsureCreateElement::EnsurePlayerController);
	EnsureSetting(EnsureCreateElement::EnsurePlayerState);
}

void APlayerCharacter::OnRep_Controller()
{
	Super::OnRep_Controller();
	if (!HasAuthority())
	{
		PlayerController = CastChecked<APC_Player>(GetController());
		NULLCHECK_RETURN_LOG(PlayerController, PlayerLog, Error, );
	}
	EnsureSetting(EnsureCreateElement::EnsurePlayerController);
}

void APlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	PS = GetPlayerState<APS_Player>();
	NULLCHECK_RETURN_LOG(PS, PlayerLog, Error, );
	PS->SetIdentifyCharacterData();
	SetMeshByCharacterType(PS);
	TPT_LOG(PlayerLog, Log, TEXT("Server Skill : %d, Client Skill : %d"), (int32)PS->IdentifyCharacterData.HostSkill, (int32)PS->IdentifyCharacterData.ClientSkill);

	ASC = PS->GetAbilitySystemComponent();
	NULLCHECK_RETURN_LOG(ASC, PlayerLog, Error, );
	ASC->InitAbilityActorInfo(PS, this);
	const UPlayerAttributeSet* AttributeSet = ASC->GetSet<UPlayerAttributeSet>();
	NULLCHECK_RETURN_LOG(AttributeSet, PlayerLog, Error, );
	BindAttributeDelegates(AttributeSet);

	InitHUDWidget(AttributeSet);
	UPlayerAnimInstance* AnimInstance = Cast<UPlayerAnimInstance>(GetMesh()->GetAnimInstance());
	AnimInstance->InitializeWithAbilitySystem(ASC);
	EnsureSetting(EnsureCreateElement::EnsurePlayerState);
}

bool APlayerCharacter::CanInteract_Implementation(const APawn* Interactor, bool bIsDetected)
{
	NULLCHECK_RETURN_LOG(ASC, PlayerLog, Error, false);
	bool bIsTag = ASC->HasMatchingGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_State_Downed);

	if (bIsTag && bIsDetected)
	{
		if (!Interactor->IsLocallyControlled())
			return true;
		InteractWidget->GetUserWidgetObject()->SetVisibility(ESlateVisibility::Visible);
		return true;
	}
	if (!Interactor->IsLocallyControlled())
		return false;
	InteractWidget->GetUserWidgetObject()->SetVisibility(ESlateVisibility::Hidden);
	return false;
}

void APlayerCharacter::OnInteractServer_Implementation(const APawn* Interactor)
{
	OnRecoveryCompleted();
	const APlayerCharacter* C = Cast<APlayerCharacter>(Interactor);
	if(C)
	{
		C->GetFocusTrace()->FocusedActor = nullptr;
	}
}

void APlayerCharacter::OnInteractClient_Implementation(const APawn* Interactor)
{

}

float APlayerCharacter::GetTime_Implementation()
{
	return Time;
}

void APlayerCharacter::CalculateGaugePercent_Implementation(float Elapsed)
{
	RecoveryPercent = Elapsed / Time;
	OnRep_RecoveryPercent();
}

void APlayerCharacter::SetHoldingGaugeUI_Implementation(const APawn* Interactor, bool bVisible)
{
	APC_Player* PC = APC_Player::GetLocalPlayerController(Interactor->GetController());

	PC->SetWidget(TEXT("RecoveryGauge"), bVisible, EMessageTargetType::Multicast);
}

void APlayerCharacter::InitPostProcessComponent()
{
	NULLCHECK_RETURN_LOG(HitVignette, PlayerLog, Error, );
	NULLCHECK_RETURN_LOG(DownedVignette, PlayerLog, Error, );
	NULLCHECK_RETURN_LOG(Confused3rdVignette, PlayerLog, Error, );
	NULLCHECK_RETURN_LOG(TrapVignette, PlayerLog, Error, );
	NULLCHECK_RETURN_LOG(MentalAttackVignette, PlayerLog, Error, );

	UMaterialInstanceDynamic* HitMID = UMaterialInstanceDynamic::Create(HitVignette, this);
	UMaterialInstanceDynamic* LowHPMID = UMaterialInstanceDynamic::Create(DownedVignette, this);
	UMaterialInstanceDynamic* Confused3rdMID = UMaterialInstanceDynamic::Create(Confused3rdVignette, this);
	UMaterialInstanceDynamic* TrapMID = UMaterialInstanceDynamic::Create(TrapVignette, this);
	UMaterialInstanceDynamic* MentalAttackMID = UMaterialInstanceDynamic::Create(MentalAttackVignette, this);

	HitBlendable.Object = HitMID;
	HitBlendable.Weight = 0.0f;

	DownedBlendable.Object = LowHPMID;
	DownedBlendable.Weight = 0.0f;

	Confused3rdBlendable.Object = Confused3rdMID;
	Confused3rdBlendable.Weight = 0.0f;

	TrapBlendable.Object = TrapMID;
	TrapBlendable.Weight = 0.0f;

	MentalAttackBlendable.Object = MentalAttackMID;
	MentalAttackBlendable.Weight = 0.0f;

	PostProcessComp->Settings.WeightedBlendables.Array.Add(HitBlendable);
	PostProcessComp->Settings.WeightedBlendables.Array.Add(DownedBlendable);
	PostProcessComp->Settings.WeightedBlendables.Array.Add(TrapBlendable);
	PostProcessComp->Settings.WeightedBlendables.Array.Add(Confused3rdBlendable);
	PostProcessComp->Settings.WeightedBlendables.Array.Add(MentalAttackBlendable);
}

void APlayerCharacter::SetHP(int32 value)
{
	NULLCHECK_RETURN_LOG(DroneUserWidget, PlayerLog, Error, );
	//TPT_LOG(HUDLog, Log, TEXT("HP : %d"), value);
	DroneUserWidget->SetHP(value);
}

void APlayerCharacter::SetMP(int32 value)
{
	NULLCHECK_RETURN_LOG(DroneUserWidget, PlayerLog, Error, );
	//TPT_LOG(HUDLog, Log, TEXT("MP : %d"), value);
	DroneUserWidget->SetMP(value);
}

void APlayerCharacter::SettingPostProcessComponentBlendable(EVignetteType Type, float Weight)
{
	NULLCHECK_RETURN_LOG(PostProcessComp, PlayerLog, Warning, );

	if (PostProcessComp->Settings.WeightedBlendables.Array.Num() <= 0)
		return;

	const int32 index = static_cast<int32>(Type) - 1;
	if (!PostProcessComp->Settings.WeightedBlendables.Array.IsValidIndex(index)) return;
	PostProcessComp->AddOrUpdateBlendable(PostProcessComp->Settings.WeightedBlendables.Array[index].Object, Weight);
}

void APlayerCharacter::InitHUDWidget(const UPlayerAttributeSet* AttributeSet)
{
	if (!AttributeSet) return;

	if (!IsLocallyControlled())
	{
		return;
	}

	if (!PlayerHUDWidget)
	{
		AUIManagerPlayerController* PC = Cast<AUIManagerPlayerController>(GetController());
		if (PC && PlayerHUDWidgetClass)
		{
			PC->RegisterWidget(TEXT("PlayerHUDWidget"), CreateWidget<UPlayerHUDWidget>(GetWorld(), PlayerHUDWidgetClass));
			PlayerHUDWidget = Cast<UPlayerHUDWidget>(PC->GetWidget(TEXT("PlayerHUDWidget")));
		}
		else
		{
			TPT_LOG(HUDLog, Error, TEXT("InitHUDWidget: Invalid PlayerController or PlayerHUDWidgetClass"));
			return;
		}
	}

	// AttributeSet에서 값 가져와 위젯 초기화 호출
	int32 HP = AttributeSet->GetMaxHP();
	int32 Mental = AttributeSet->GetMaxMentalPoint();
	int32 Stamina = AttributeSet->GetMaxStamina();
	int32 CoreEnergy = AttributeSet->GetCoreEnergy();

	PlayerHUDWidget->InitializeWidgets(HP, Mental, Stamina, CoreEnergy);

	HealthPoint = HP;
	MentalPoint = Mental;

	bool bIsComplete = PS->InventoryComp->SetPlayerHUDWidget(PlayerHUDWidget);

	if (bIsComplete)
	{
		PS->InventoryComp->RefreshUIFromInventory();
	}

	if (DroneUserWidget)
	{
		SetHP(HealthPoint);
		SetMP(MentalPoint);
	}
}

void APlayerCharacter::DroneWidgetOnOff(bool Visibility)
{
	if (Visibility)
	{
		DroneWidget->GetUserWidgetObject()->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		DroneWidget->GetUserWidgetObject()->SetVisibility(ESlateVisibility::Hidden);
	}
}

void APlayerCharacter::PlayerHUDStaminaSet(int32 value)
{
	NULLCHECK_RETURN_LOG(PlayerHUDWidget, HUDLog, Error, );
	PlayerHUDWidget->UpdateStamina(value);
	const UPlayerAttributeSet* AttributeSet = ASC->GetSet<UPlayerAttributeSet>();
	if (AttributeSet->GetMaxStamina() != value)
		PlayerHUDWidget->VisibleStamina(true);
}

void APlayerCharacter::HidePlayerHUDStaminaSet(int32 value)
{
	GetWorldTimerManager().ClearTimer(VisibleStaminaTimerHandle); // 중복 타이머 방지

	GetWorldTimerManager().SetTimer(
		VisibleStaminaTimerHandle,
		[this]()
		{
			if (PlayerHUDWidget)
			{
				PlayerHUDWidget->VisibleStamina(false);
			}
		},
		1.0f,
		false
	);
}

void APlayerCharacter::PlayerHUDCoreEnergySet(int32 value)
{
	NULLCHECK_RETURN_LOG(PlayerHUDWidget, HUDLog, Error, );
	PlayerHUDWidget->UpdateCoreEnergy(value);
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// 로컬에서만 일어남.
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	UTPTEnhancedInputComponent* TPTInput = CastChecked<UTPTEnhancedInputComponent>(PlayerInputComponent);
	check(TPTInput);
	TPTInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ThisClass::Move);
	TPTInput->BindAction(LookAction, ETriggerEvent::Triggered, this, &ThisClass::Look);
	TPTInput->BindAction(MouseWheelUpAction, ETriggerEvent::Triggered, this, &ThisClass::InputMouseWheelUp);
	TPTInput->BindAction(MouseWheelDownAction, ETriggerEvent::Triggered, this, &ThisClass::InputMouseWheelDown);
	TPTInput->BindAction(ESC, ETriggerEvent::Started, this, &ThisClass::InputESC);
	TPTInput->BindAction(TabAction, ETriggerEvent::Started, this, &APlayerCharacter::InputTab);

	SetupPlayerInputByTag(TPTInput);
}

void APlayerCharacter::SetupPlayerInputByTag(UTPTEnhancedInputComponent* TPTInput)
{
	if (IsValid(TPTInput))
	{
		TPTInput->BindActionByTag(InputConfig, FTPTGameplayTags::Get().TPTGameplay_InputTag_Player_Run, ETriggerEvent::Started, this, &ThisClass::InputPressed);
		TPTInput->BindActionByTag(InputConfig, FTPTGameplayTags::Get().TPTGameplay_InputTag_Player_Run, ETriggerEvent::Completed, this, &ThisClass::InputReleased);
		TPTInput->BindActionByTag(InputConfig, FTPTGameplayTags::Get().TPTGameplay_InputTag_Player_Crouch, ETriggerEvent::Triggered, this, &ThisClass::InputPressed);

		TPTInput->BindActionByTag(InputConfig, FTPTGameplayTags::Get().TPTGameplay_InputTag_Player_Interact, ETriggerEvent::Started, this, &ThisClass::InputPressed);
		TPTInput->BindActionByTag(InputConfig, FTPTGameplayTags::Get().TPTGameplay_InputTag_Player_Interact, ETriggerEvent::Completed, this, &ThisClass::InputReleased);
		TPTInput->BindActionByTag(InputConfig, FTPTGameplayTags::Get().TPTGameplay_InputTag_Player_LookBack, ETriggerEvent::Started, this, &ThisClass::InputPressed);

		TPTInput->BindActionByTag(InputConfig, FTPTGameplayTags::Get().TPTGameplay_InputTag_Player_ActiveSkill_Q, ETriggerEvent::Started, this, &ThisClass::InputPressed);
		TPTInput->BindActionByTag(InputConfig, FTPTGameplayTags::Get().TPTGameplay_InputTag_Player_ActiveSkill_E, ETriggerEvent::Started, this, &ThisClass::InputPressed);
		TPTInput->BindActionByTag(InputConfig, FTPTGameplayTags::Get().TPTGameplay_InputTag_Player_ItemSlot_1st, ETriggerEvent::Started, this, &ThisClass::InputPressedWithNum, 1);
		TPTInput->BindActionByTag(InputConfig, FTPTGameplayTags::Get().TPTGameplay_InputTag_Player_ItemSlot_2nd, ETriggerEvent::Started, this, &ThisClass::InputPressedWithNum, 2);
		TPTInput->BindActionByTag(InputConfig, FTPTGameplayTags::Get().TPTGameplay_InputTag_Player_ItemSlot_3rd, ETriggerEvent::Started, this, &ThisClass::InputPressedWithNum, 3);
		TPTInput->BindActionByTag(InputConfig, FTPTGameplayTags::Get().TPTGameplay_InputTag_Player_ItemSlot_4th, ETriggerEvent::Started, this, &ThisClass::InputPressedWithNum, 4);
		TPTInput->BindActionByTag(InputConfig, FTPTGameplayTags::Get().TPTGameplay_InputTag_Player_ItemSlot_5th, ETriggerEvent::Started, this, &ThisClass::InputPressedWithNum, 5);
		TPTInput->BindActionByTag(InputConfig, FTPTGameplayTags::Get().TPTGameplay_InputTag_Player_UseItem, ETriggerEvent::Started, this, &ThisClass::InputPressedUseItem);
	}
}

void APlayerCharacter::OnAbilityFailed(const UGameplayAbility* Ability, const FGameplayTagContainer& FailureTags)
{
	NULLCHECK_RETURN_LOG(Ability, PlayerLog, Error, );
	TPT_LOG(PlayerLog, Warning, TEXT("Ability Failed: %s"), *Ability->GetName());
	for (const FGameplayTag& Tag : FailureTags)
	{
		TPT_LOG(PlayerLog, Warning, TEXT(" - Failure Tag: %s"), *Tag.ToString());
	}

	UE_LOG(LogTemp, Warning, TEXT("Ability %s failed to activate. Reason: %s"),
		*Ability->GetName(),
		*FailureTags.ToString());
}

void APlayerCharacter::ExecuteAbilityByTag(FGameplayTag InputTag)
{
	NULLCHECK_RETURN_LOG(ASC, PlayerLog, Warning, );

	ASC->AddLooseGameplayTag(InputTag); // 로컬에게 비네팅 적용 됨. 다운드 애니메이션 적용 안됨. 서버에선 엎어짐. 서버에만 태그가 붙음.(당연함)
	ASC->AddReplicatedLooseGameplayTag(InputTag); // 리플리케이트로 붙이면 서버에서는 아무것도 적용안됨. 로컬에서는 다 됨. 태그도 로컬에만 존재함.

	bool bActivated = ASC->TryActivateAbilitiesByTag(FGameplayTagContainer(InputTag));
	if(!bActivated)
	{
		TPT_LOG(PlayerLog, Warning, TEXT("Ability activation failed.%s"), *InputTag.ToString());
		//ASC->TryActivateAbilitiesByTag(FGameplayTagContainer(InputTag));
	}

	if(InputTag == FTPTGameplayTags::Get().TPTGameplay_Character_State_Downed)
	{
		S2A_OnDownedWidget(true);
	}
}

void APlayerCharacter::BindAttributeDelegates(const UPlayerAttributeSet* AttributeSet)
{
	if (HasAuthority())
	{
		
		AttributeSet->OnPlayerDamaged.AddDynamic(this, &ThisClass::ExecuteAbilityByTag);
		AttributeSet->OnPlayerLowHP.AddDynamic(this, &ThisClass::ExecuteAbilityByTag);
		AttributeSet->OnPlayerDowned.AddDynamic(this, &ThisClass::ExecuteAbilityByTag);
		AttributeSet->OnPlayerConfused.AddDynamic(this, &ThisClass::ExecuteAbilityByTag);
		AttributeSet->OnPlayerUseSkill.AddDynamic(this, &ThisClass::ExecuteAbilityByTag);
		AttributeSet->OnMentalPointNotMax.AddDynamic(this, &ThisClass::ExecuteAbilityByTag);

		WallSina->OnComponentBeginOverlap.AddDynamic(this, &APlayerCharacter::OnBeginOverlapWall);
		WallSina->OnComponentEndOverlap.AddDynamic(this, &APlayerCharacter::OnEndOverlapWall);
		WallRose->OnComponentBeginOverlap.AddDynamic(this, &APlayerCharacter::OnBeginOverlapWall);
		WallRose->OnComponentEndOverlap.AddDynamic(this, &APlayerCharacter::OnEndOverlapWall);
		WallMaria->OnComponentBeginOverlap.AddDynamic(this, &APlayerCharacter::OnBeginOverlapWall);
		WallMaria->OnComponentEndOverlap.AddDynamic(this, &APlayerCharacter::OnEndOverlapWall);
	}
	//AttributeSet->OnChangedSpeed.AddDynamic(this, &ThisClass::SpeedSetting);  //GA에서 직접하는것보다 동기화가 늦는다. 이유는 모름.
	ASC->RegisterGameplayTagEvent(FTPTGameplayTags::Get().TPTGameplay_Character_State_AIChasing).AddUObject(this, &ThisClass::OnTagChanged);
	ASC->AbilityFailedCallbacks.AddUObject(this, &ThisClass::OnAbilityFailed);
	if (IsLocallyControlled())
	{	
		AttributeSet->OnChangedStamina.AddDynamic(this, &ThisClass::PlayerHUDStaminaSet);
		AttributeSet->OnFullStamina.AddDynamic(this, &ThisClass::HidePlayerHUDStaminaSet);
		AttributeSet->OnChangedCoreEnergy.AddDynamic(this, &ThisClass::PlayerHUDCoreEnergySet);
		AttributeSet->OnChangedHP.AddDynamic(this, &APlayerCharacter::SetHP);
		AttributeSet->OnChangedMentalPoint.AddDynamic(this, &APlayerCharacter::SetMP);
	}
}

void APlayerCharacter::OnTagChanged(const FGameplayTag InputTag, int32 Count)
{
	if (Count)
	{
		ASC->TryActivateAbilitiesByTag(FGameplayTagContainer(InputTag));
	}
}

void APlayerCharacter::OnRecoveryCompleted()
{
	NULLCHECK_RETURN_LOG(ASC, PlayerLog, Error, );

	FGameplayTag DownedTag = FTPTGameplayTags::Get().TPTGameplay_Character_State_Downed;
	FGameplayTag ConfusedTag = FTPTGameplayTags::Get().TPTGameplay_Character_State_Confused3rd;
	FGameplayTag LowHPTag = FTPTGameplayTags::Get().TPTGameplay_Character_State_LowHP;

	int32 DownedTagCount = ASC->GetTagCount(DownedTag);
	for (int32 i = 0; i < DownedTagCount; ++i)
	{
		ASC->RemoveLooseGameplayTag(DownedTag);
		ASC->RemoveReplicatedLooseGameplayTag(DownedTag);
	}

	if (ASC->HasMatchingGameplayTag(LowHPTag))
	{
		ASC->RemoveLooseGameplayTag(LowHPTag);
		ASC->RemoveReplicatedLooseGameplayTag(LowHPTag);
	}

	if (ASC->HasMatchingGameplayTag(ConfusedTag))
	{
		ASC->RemoveLooseGameplayTag(ConfusedTag);
		ASC->RemoveReplicatedLooseGameplayTag(ConfusedTag);
	}

	InteractWidget->GetUserWidgetObject()->SetVisibility(ESlateVisibility::Hidden);
	S2A_OnDownedWidget(false);

	APC_Player* PC = APC_Player::GetLocalPlayerController(this);

	PC->SetWidget(TEXT("RecoveryGauge"), false, EMessageTargetType::Multicast);
	PC->SetWidget(TEXT("WASD"), false, EMessageTargetType::LocalClient);

	if (HasAuthority())
	{
		if (AGM_PhantomTwins* GM = GetWorld()->GetAuthGameMode<AGM_PhantomTwins>())
		{
			GM->NotifyPlayerDied(false);
		}
	}

	if (RecoveryGE)
	{
		FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
		ContextHandle.AddSourceObject(this);
		FGameplayEffectSpecHandle EffectSpecHandle = ASC->MakeOutgoingSpec(RecoveryGE, 1.0f, ContextHandle);
		if (EffectSpecHandle.IsValid())
		{
			ASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
		}
	}

	//SpringArm->SocketOffset += FVector(0.f, 0.f, 100.f);
}

void APlayerCharacter::InputPressed(int32 InputID)
{
	NULLCHECK_RETURN_LOG(ASC, PlayerLog, Warning, );
	FGameplayAbilitySpec* Spec = ASC->FindAbilitySpecFromInputID(InputID);
	NULLCHECK_RETURN_LOG(Spec,PlayerLog, Warning,);

	if (Spec->IsActive())
	{
		ASC->AbilitySpecInputPressed(*Spec);
	}
	else
	{
		ASC->TryActivateAbility(Spec->Handle);
	}
	// 스태미나 위젯 조정.
	const EFTPTGameplayTags* TagEnum = FTPTGameplayTags::Get().TagMap.Find(FTPTGameplayTags::Get().TPTGameplay_InputTag_Player_Run);
	int32 InputNum = static_cast<int32>(*TagEnum);

	FGameplayTag DownedTag = FTPTGameplayTags::Get().TPTGameplay_Character_State_Downed;
	FGameplayTag ConfusedTag = FTPTGameplayTags::Get().TPTGameplay_Character_State_Confused3rd;
	if (!ASC->HasMatchingGameplayTag(DownedTag) && !ASC->HasMatchingGameplayTag(ConfusedTag) && InputID == InputNum)
	{
		PlayerHUDWidget->VisibleStamina(true);
	}
}

void APlayerCharacter::InputSKillPressed(int32 InputID, int32 SkillNumber)
{
	FGameplayTag EventTag = FTPTGameplayTags::Get().TPTGameplay_Character_Skill_ActiveSkill;
	FGameplayEventData Payload;
	Payload.EventTag = EventTag;
	Payload.Instigator = this;
	Payload.EventMagnitude = static_cast<float>(SkillNumber);

	ASC->HandleGameplayEvent(EventTag, &Payload);
}

void APlayerCharacter::InputPressedWithNum(int32 InputID, int32 SlotNumber)
{
	SelectedSlotNumber = SlotNumber;

	PlayerHUDWidget->VisibleInventory(true);

	// 5초 뒤에 인벤토리(UI) 비활성화
	GetWorldTimerManager().ClearTimer(VisibleInventoryTimerHandle); // 중복 타이머 방지
	GetWorld()->GetTimerManager().SetTimer(
		VisibleInventoryTimerHandle,
		FTimerDelegate::CreateWeakLambda(this, [this]()
			{
				if (PlayerHUDWidget)
				{
					PlayerHUDWidget->VisibleInventory(false);
				}
			}),
		5.0f, // 초 단위
		false // 반복 아님
	);

	FGameplayTag EventTag = FTPTGameplayTags::Get().TPTGameplay_Event_Character_HoldItem;
	FGameplayEventData Payload;
	Payload.EventTag = EventTag;
	Payload.Instigator = this;
	Payload.EventMagnitude = static_cast<float>(SlotNumber);

	ASC->HandleGameplayEvent(EventTag, &Payload);
}

void APlayerCharacter::InputMouseWheelDown(const FInputActionValue& Value)
{
	SelectedSlotNumber++;

	// 인벤토리 슬롯 개수 넘으면 처음으로
	if (SelectedSlotNumber > MaxSlotNumber)
	{
		SelectedSlotNumber = 1;
	}

	InputPressedWithNum(0, SelectedSlotNumber);
}

void APlayerCharacter::InputESC(const FInputActionValue& Value)
{
	APC_Player* PC = APC_Player::GetLocalPlayerController(this);

	PC->SetWidget(TEXT("ESC"),	true, EMessageTargetType::LocalClient);
	bIsShowingESC = true;
	FInputModeUIOnly InputData;
	PC->SetInputMode(InputData);
	PC->bShowMouseCursor = true;
}

void APlayerCharacter::InputTab(const FInputActionValue& Value)
{
	NULLCHECK_RETURN_LOG(DroneWidget, PlayerLog, Error, );
	NULLCHECK_RETURN_LOG(DroneUserWidget, PlayerLog, Error, );

	if (!IsLocallyControlled()) return;

	if (DroneWidget->GetUserWidgetObject()->GetVisibility() == ESlateVisibility::Visible)
	{
		DroneUserWidget->CloseAnimation(this);
	}
	else if (DroneWidget->GetUserWidgetObject()->GetVisibility() == ESlateVisibility::Hidden)
	{
		DroneUserWidget->OpenAnimation(this);
	}
}

void APlayerCharacter::InputMouseWheelUp(const FInputActionValue& Value)
{
	SelectedSlotNumber--;

	// 인벤토리 슬롯 개수보다 적으면 끝으로
	if (SelectedSlotNumber < 1)
	{
		SelectedSlotNumber = MaxSlotNumber;
	}

	InputPressedWithNum(0, SelectedSlotNumber);
}

void APlayerCharacter::InputPressedUseItem(int32 InputID)
{
	if(ASC->HasMatchingGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_State_Hide))
	{
		return;
	}

	UInventoryComponent* InventoryComponent = PS->InventoryComp;

	if (InventoryComponent)
	{
		InventoryComponent->UseItem(SelectedSlotNumber);
	}

	if (HasAuthority())
	{
		// 서버에서 실행 시 → 바로 제거 + 멀티캐스트
		RemoveHeldItemMesh();
		S2A_RemoveHeldItemMesh();
	}
	else
	{
		// 클라에서 실행 시 → 서버에 요청
		C2S_RemoveHeldItemMesh();
		// 동시에 자기 로컬 메쉬도 제거
		RemoveHeldItemMesh();
	}

	SelectedSlotNumber = 0;
}

void APlayerCharacter::InputReleased(int32 InputID)
{
	NULLCHECK_RETURN_LOG(ASC, PlayerLog, Error, );
	FGameplayAbilitySpec* Spec = ASC->FindAbilitySpecFromInputID(InputID);
	Spec->InputPressed = false;
	if (!HasAuthority())
	{
		C2S_InputReleased(InputID);
	}
	else if (Spec->IsActive())
	{
		ASC->AbilitySpecInputReleased(*Spec);
	}

	const EFTPTGameplayTags* TagEnum = FTPTGameplayTags::Get().TagMap.Find(FTPTGameplayTags::Get().TPTGameplay_InputTag_Player_Run);
	int32 InputNum = static_cast<int32>(*TagEnum);
	const UPlayerAttributeSet* AttributeSet = ASC->GetSet<UPlayerAttributeSet>();
	if (InputID == InputNum && AttributeSet->GetStamina() >= AttributeSet->GetMaxStamina())
	{
		HidePlayerHUDStaminaSet(0);
	}
}

void APlayerCharacter::C2S_InputReleased_Implementation(const int32 InputID)
{
	FGameplayAbilitySpec* Spec = ASC->FindAbilitySpecFromInputID(InputID);
	if (!Spec) return;

	Spec->InputPressed = false;

	if (Spec->IsActive())
	{
		ASC->AbilitySpecInputReleased(*Spec);
	}
}

void APlayerCharacter::C2S_SetFocusTrace_Implementation(const FVector& CameraLocation, const FRotator& CameraRotation)
{
	if (HasAuthority() && FocusTrace)
	{
		FocusTrace->SetStart(CameraLocation);

		FVector Direction = CameraRotation.Vector(); // 카메라 회전 기반 전방 방향 벡터를 다시 구함
		FocusTrace->SetDirection(Direction);

		FocusTrace->SetCollisionType(ECC_GameTraceChannel1);
	}
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
	float MouseSensitivity = 0.3f;
	FVector2D LookAxisVector = Value.Get<FVector2D>() * MouseSensitivity;
	AddControllerYawInput(LookAxisVector.X);
	AddControllerPitchInput(LookAxisVector.Y);
}

void APlayerCharacter::MovementSetting()
{
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// movement setting
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0, 500, 0);
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 0.f;
	GetCharacterMovement()->MaxWalkSpeedCrouched = 80.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 600.f;
}
void APlayerCharacter::SpeedSetting(int32 Speed)
{
	GetCharacterMovement()->MaxWalkSpeed = Speed;
}
void APlayerCharacter::CameraSetting()
{
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->bEnableCameraLag = true;
	SpringArm->TargetArmLength = 100.0f;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->bUsePawnControlRotation = false;
	Camera->SetupAttachment(SpringArm);
}

void APlayerCharacter::OverlapRangeSetting()
{
	WallSina = CreateDefaultSubobject<USphereComponent>(TEXT("WallSina"));
	WallSina->SetupAttachment(RootComponent);
	WallSina->SetSphereRadius(500.f);
	WallSina->SetCollisionEnabled(ECollisionEnabled::QueryOnly);  // 모든 채널 무시
	WallSina->SetCollisionResponseToAllChannels(ECR_Ignore);
	WallSina->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);  // Pawn 채널(플레이어, AI 등)만 오버랩!

	WallRose = CreateDefaultSubobject<USphereComponent>(TEXT("WallRose"));
	WallRose->SetupAttachment(RootComponent);
	WallRose->SetSphereRadius(1000.f);
	WallRose->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	WallRose->SetCollisionResponseToAllChannels(ECR_Ignore);
	WallRose->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	WallMaria = CreateDefaultSubobject<USphereComponent>(TEXT("WallMaria"));
	WallMaria->SetupAttachment(RootComponent);
	WallMaria->SetSphereRadius(1500.f);
	WallMaria->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	WallMaria->SetCollisionResponseToAllChannels(ECR_Ignore);
	WallMaria->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void APlayerCharacter::OnBeginOverlapWall(UPrimitiveComponent* Comp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Comp == WallSina) OnBeginOverlap(EEnemyRange::WallSina, OtherActor);
	else if (Comp == WallRose) OnBeginOverlap(EEnemyRange::WallRose, OtherActor);
	else if (Comp == WallMaria) OnBeginOverlap(EEnemyRange::WallMaria, OtherActor);
}

void APlayerCharacter::OnEndOverlapWall(UPrimitiveComponent* Comp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (Comp == WallSina) OnEndOverlap(EEnemyRange::WallSina, OtherActor);
	else if (Comp == WallRose) OnEndOverlap(EEnemyRange::WallRose, OtherActor);
	else if (Comp == WallMaria) OnEndOverlap(EEnemyRange::WallMaria, OtherActor);
}

void APlayerCharacter::OnBeginOverlap(EEnemyRange Range, AActor* OtherActor)
{
	UAbilitySystemComponent* AIASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OtherActor);
	if (AIASC)
	{
		bool AIHasTag = AIASC->HasMatchingGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_Identifier_AI);
		if (AIHasTag)
		{
			EEnemyRange* Found = EnemyRangeMap.Find(OtherActor);
			if (!Found || Range < *Found)
			{
				EnemyRangeMap.Add(OtherActor, Range);
			}
		}
		SetNearestEnemyRange();
	}
}

void APlayerCharacter::OnEndOverlap(EEnemyRange Range, AActor* OtherActor)
{
	UAbilitySystemComponent* AIASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OtherActor);
	if (AIASC && AIASC->HasMatchingGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_Identifier_AI))
	{
		EEnemyRange* Found = EnemyRangeMap.Find(OtherActor);
		if (Found && *Found == Range)
		{
			// 더 바깥 반경에 있는지 체크
			if (Range == EEnemyRange::WallSina)
			{
				if (WallRose->IsOverlappingActor(OtherActor))
					*Found = EEnemyRange::WallRose;
				else if (WallMaria->IsOverlappingActor(OtherActor))
					*Found = EEnemyRange::WallMaria;
				else
					EnemyRangeMap.Remove(OtherActor);
			}
			else if (Range == EEnemyRange::WallRose)
			{
				if (WallMaria->IsOverlappingActor(OtherActor))
					*Found = EEnemyRange::WallMaria;
				else
					EnemyRangeMap.Remove(OtherActor);
			}
			else
			{
				EnemyRangeMap.Remove(OtherActor);
			}
		}
	}
	SetNearestEnemyRange();
}

void APlayerCharacter::SetNearestEnemyRange()
{
	EEnemyRange Nearest = EEnemyRange::None;
	for (const auto& Pair : EnemyRangeMap)
	{
		if (Nearest == EEnemyRange::None || Pair.Value < Nearest)
			Nearest = Pair.Value;
	}
	if (Nearest != CurrentWallRange)
	{
		CurrentWallRange = Nearest;
		if (HasAuthority())
		{
			UpdateWallSound();
		}
	}
}

void APlayerCharacter::UpdateWallSound()
{
	if (!IsLocallyControlled())
	{
		return;
	}

	// 이전 사운드 중단
	if (WallAudioComponent)
	{
		WallAudioComponent->Stop();
		WallAudioComponent = nullptr;
	}

	// 새로운 범위 Enum에 맞는 사운드 재생
	USoundBase* ToPlay = nullptr;
	switch (CurrentWallRange)
	{
	case EEnemyRange::WallSina:  ToPlay = WallSinaSound;   break;
	case EEnemyRange::WallRose:  ToPlay = WallRoseSound;   break;
	case EEnemyRange::WallMaria: ToPlay = WallMariaSound;  break;
	default: break;
	}

	NULLCHECK_RETURN_LOG(ASC, PlayerLog, Warning, );
	if (ToPlay && !ASC->HasMatchingGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_State_Confused3rd))
	{
		WallAudioComponent = UGameplayStatics::SpawnSoundAttached(ToPlay, GetRootComponent());
	}
}

void APlayerCharacter::OnRep_CurrentWallRange()
{
	UpdateWallSound();
}

void APlayerCharacter::EnsureSetting(EnsureCreateElement Element)
{
	bool bStart = true;
	bEnsureSet[Element] = true;
	
	for (bool node : bEnsureSet)
	{
		if (!node)
		{
			bStart = false;
			break;
		}
	}

	if (bStart && !bOnceTime)
	{
		bOnceTime = true;
		EnsureGameStart();
	}
}

void APlayerCharacter::EnsureGameStart_Implementation()
{
	NULLCHECK_RETURN_LOG(ASC, PlayerLog, Error, );
	NULLCHECK_RETURN_LOG(PlayerController, PlayerLog, Error, );
	NULLCHECK_RETURN_LOG(PS, PlayerLog, Error, );
	NULLCHECK_RETURN_LOG(DroneWidget, PlayerLog, Error, );
	NULLCHECK_RETURN_LOG(DroneWidgetClass, PlayerLog, Error, );

	const UPlayerAttributeSet* AttributeSet = ASC->GetSet<UPlayerAttributeSet>();
	NULLCHECK_RETURN_LOG(AttributeSet, PlayerLog, Error, );

	SetSelectSkill(PS);

	PlayerController->SetWidget(TEXT("PlayerHUDWidget"), true, EMessageTargetType::LocalClient);

	DroneMesh = FindComponentByTag<USkeletalMeshComponent>(TEXT("DroneMesh"));
	if (DroneMesh)
	{
		DroneWidget->AttachToComponent(DroneMesh, FAttachmentTransformRules::KeepRelativeTransform);
		TPT_LOG(PlayerLog, Log, TEXT("Attach Complete! : %s"), *DroneWidget->GetAttachParent()->GetName());
	}

	UUserWidget* Drone = CreateWidget(GetWorld(), DroneWidgetClass);
	DroneWidget->SetWidget(Drone);
	DroneWidget->GetUserWidgetObject()->SetVisibility(ESlateVisibility::Hidden);
	DroneWidget->SetCastShadow(false);

	if (Drone)
	{
		DroneUserWidget = Cast<UDroneStatWidget>(Drone);
	}

	if (IsLocallyControlled())
	{
		//NULLCHECK_RETURN_LOG(RecoveryWidgetClass, PlayerLog, Error, );
		PlayerController->RegisterWidget(TEXT("RecoveryGauge"), CreateWidget<UUserWidget>(GetWorld(), RecoveryWidgetClass));
		//NULLCHECK_RETURN_LOG(KeyWidgetClass, PlayerLog, Error, );
		PlayerController->RegisterWidget(TEXT("WASD"), CreateWidget<UUserWidget>(GetWorld(), KeyWidgetClass));
		//NULLCHECK_RETURN_LOG(CannotUseItemWidgetClass, PlayerLog, Error, );
		PlayerController->RegisterWidget(TEXT("CannotUseItem"), CreateWidget<UUserWidget>(GetWorld(), CannotUseItemWidgetClass));
		//NULLCHECK_RETURN_LOG(GameOverWidgetClass, PlayerLog, Error, );
		PlayerController->RegisterWidget(TEXT("GameOver"), CreateWidget(GetWorld(), GameOverWidgetClass));
		//NULLCHECK_RETURN_LOG(LoadingWidgetClass, PlayerLog, Error, );
		//PC->RegisterWidget(TEXT("Loading"), CreateWidget(GetWorld(), LoadingWidgetClass));
		//NULLCHECK_RETURN_LOG(ESCWidgetClass, PlayerLog, Error, );
		PlayerController->RegisterWidget(TEXT("ESC"), CreateWidget(GetWorld(), ESCWidgetClass));
		//NULLCHECK_RETURN_LOG(GameStopWidgetClass, PlayerLog, Error, );
		PlayerController->RegisterWidget(TEXT("GameStop"), CreateWidget(GetWorld(), GameStopWidgetClass));
		//NULLCHECK_RETURN_LOG(ResumeCountWidgetClass, PlayerLog, Error, );
		PlayerController->RegisterWidget(TEXT("ResumeCount"), CreateWidget(GetWorld(), ResumeCountWidgetClass));

		if (DroneUserWidget)
		{
			SetHP(AttributeSet->GetMaxHP());
			SetMP(AttributeSet->GetMaxMentalPoint());
		}
		else
		{
			TPT_LOG(PlayerLog, Error, TEXT("InitHUDWidget: DroneUserWidget is null"));
		}
	}
}

void APlayerCharacter::RemoveHeldItemMesh()
{
	USkeletalMeshComponent* MeshComp = GetMesh();
	if (!MeshComp) return;

	const FName HandSocketName = TEXT("RightHandSocket");

	// 캐릭터 Mesh에 붙은 자식 컴포넌트 전부 탐색
	TArray<USceneComponent*> AttachedComponents;
	MeshComp->GetChildrenComponents(true, AttachedComponents);

	for (USceneComponent* Comp : AttachedComponents)
	{
		if (!Comp) continue;

		// 손 소켓에 붙은 StaticMeshComponent만 제거
		if (Comp->GetAttachSocketName() == HandSocketName)
		{
			if (UStaticMeshComponent* HeldMeshComp = Cast<UStaticMeshComponent>(Comp))
			{
				HeldMeshComp->DestroyComponent();
				UE_LOG(LogTemp, Log, TEXT("Removed HeldItem StaticMesh from RightHandSocket"));
				return; // 하나만 제거하면 되므로 바로 종료
			}
		}
	}
}

void APlayerCharacter::C2S_RemoveHeldItemMesh_Implementation()
{
	// 서버에서 제거
	RemoveHeldItemMesh();

	// 모든 클라(호출자 포함)에게 동기화
	S2A_RemoveHeldItemMesh();
}

void APlayerCharacter::S2A_RemoveHeldItemMesh_Implementation()
{
	RemoveHeldItemMesh();
}

void APlayerCharacter::S2A_OnDownedWidget_Implementation(bool Visible)
{
	NULLCHECK_RETURN_LOG(DownedWidget, PlayerLog, Warning, );

	if (Visible)
	{
		DownedWidget->GetUserWidgetObject()->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		DownedWidget->GetUserWidgetObject()->SetVisibility(ESlateVisibility::Hidden);
	}
}

void APlayerCharacter::GivePassiveSkillBySkillType(ESkillType Type)
{
	NULLCHECK_RETURN_LOG(ASC, PlayerLog, Error, );

	FGameplayEventData Payload;
	Payload.EventTag = FTPTGameplayTags::Get().TPTGameplay_Character_Skill_StarterKit;
	Payload.Instigator = this;

	switch (Type)
	{
	case ESkillType::NoneSkill:
		TPT_LOG(PlayerLog, Log, TEXT("Player NoneSkill"));
		break;
	case ESkillType::GiveEMP:
		Payload.EventMagnitude = static_cast<float>(EItemType::EMP);
		ASC->HandleGameplayEvent(Payload.EventTag, &Payload);
		TPT_LOG(PlayerLog, Log, TEXT("Player Give EMP"));
		break;
	case ESkillType::GiveNoiseBomb:
		Payload.EventMagnitude = static_cast<float>(EItemType::NoiseBomb);
		ASC->HandleGameplayEvent(Payload.EventTag, &Payload);
		TPT_LOG(PlayerLog, Log, TEXT("Player Give NoiseBomb"));
		break;
	case ESkillType::GiveHealPack:
		Payload.EventMagnitude = static_cast<float>(EItemType::HealPack);
		ASC->HandleGameplayEvent(Payload.EventTag, &Payload);
		TPT_LOG(PlayerLog, Log, TEXT("Player Give HealPack"));
		break;
	case ESkillType::GiveMentalPack:
		Payload.EventMagnitude = static_cast<float>(EItemType::MentalPack);
		ASC->HandleGameplayEvent(Payload.EventTag, &Payload);
		TPT_LOG(PlayerLog, Log, TEXT("Player Give MentalPack"));
		break;
	case ESkillType::GiveKey:
		Payload.EventMagnitude = static_cast<float>(EItemType::Key);
		ASC->HandleGameplayEvent(Payload.EventTag, &Payload);
		TPT_LOG(PlayerLog, Log, TEXT("Player Give Key"));
		break;
	case ESkillType::MaxHealthUp:
		ExecuteAbilityByTag(FTPTGameplayTags::Get().TPTGameplay_Character_Skill_HPBuff);
		TPT_LOG(PlayerLog, Log, TEXT("Player HP Buff"));
		break;
	case ESkillType::MaxMentalUp:
		ExecuteAbilityByTag(FTPTGameplayTags::Get().TPTGameplay_Character_Skill_MentalBuff);
		TPT_LOG(PlayerLog, Log, TEXT("Player Mental Buff"));
		break;
	case ESkillType::MaxStaminaUp:
		ExecuteAbilityByTag(FTPTGameplayTags::Get().TPTGameplay_Character_Skill_StaminaBuff);
		TPT_LOG(PlayerLog, Log, TEXT("Player Stamina Buff"));
		break;
	default:
		break;
	}
}

void APlayerCharacter::UpdateSprintCooldownCount()
{
	float SprintTotal = 0.f;
	float SprintStartTime = 0.f;

	float CooldownTotal = 0.f;
	float CooldownStartTime = 0.f;

	if (ASC)
	{
		const FActiveGameplayEffectsContainer& Effects = ASC->GetActiveGameplayEffects();

		FGameplayTag SprintTag = FTPTGameplayTags::Get().TPTGameplay_Character_State_Sprinting;
		FGameplayTag CooldownTag = FTPTGameplayTags::Get().TPTGameplay_Character_State_SprintCoolDown;

		if (Effects.GetNumGameplayEffects() <= 0)
			return;

		for (auto It = Effects.CreateConstIterator(); It; ++It)
		{
			if (!It) continue;

			const FActiveGameplayEffect& Effect = *It;
			if (!Effect.Spec.Def) continue;

			if (Effect.Spec.Def->GetGrantedTags().HasTag(SprintTag))
			{
				SprintTotal = Effect.GetDuration();
				SprintStartTime = Effect.GetTimeRemaining(GetWorld()->GetTimeSeconds());
			}
			else if (Effect.Spec.Def->GetGrantedTags().HasTag(CooldownTag))
			{
				CooldownTotal = Effect.GetDuration(); 
				CooldownStartTime = Effect.GetTimeRemaining(GetWorld()->GetTimeSeconds());
			}
		}
	}
	
	CooldownTotal -= SkillCoolTime_Q;

	//if (HasAuthority())
		//TPT_LOG(LogTemp, Error, TEXT("SprintTotal: %.2f| SprintStartTime: %.2f| CooldownTotal: %.2f| CooldownStartTime: %.2f|"), SprintTotal, SprintStartTime, CooldownTotal, CooldownStartTime);

	float SprintPercent = 0;
	float CooldownPercent = 0;

	//percent 연산
	// Sprint 퍼센트 계산: 1 → 0
	if (SprintTotal > 0.f)
	{
		SprintPercent = FMath::Clamp((SprintStartTime / SprintTotal), 0.f, 1.f);
	}

	if (SprintPercent <= 0.f) // Sprint 끝났으면
	{
		CooldownPercent = FMath::Clamp(1.f - (CooldownStartTime / CooldownTotal), 0.f, 1.f);
	}
	else
	{
		CooldownPercent = 0.f; // Sprint 진행 중이면 0
	}

	OnSprintSkillUI.Broadcast(SprintPercent, CooldownPercent);
}

void APlayerCharacter::UpdateAuraCooldownCount()
{
	float AuraTotal = 0.f;
	float AuraStartTime = 0.f;

	float CooldownTotal = 0.f;
	float CooldownStartTime = 0.f;

	if (ASC)
	{
		const FActiveGameplayEffectsContainer& Effects = ASC->GetActiveGameplayEffects();

		if (Effects.GetNumGameplayEffects() <= 0)
			return;

		FGameplayTag SprintTag = FTPTGameplayTags::Get().TPTGameplay_Character_State_UsingOutLine;
		FGameplayTag CooldownTag = FTPTGameplayTags::Get().TPTGameplay_Character_State_OutLineCoolDown;

		for (auto It = Effects.CreateConstIterator(); It; ++It)
		{
			if (!It) continue;

			const FActiveGameplayEffect& Effect = *It;
			if (!Effect.Spec.Def) continue;

			if (Effect.Spec.Def->GetGrantedTags().HasTag(SprintTag))
			{
				AuraTotal = Effect.GetDuration();
				AuraStartTime = Effect.GetTimeRemaining(GetWorld()->GetTimeSeconds());
			}
			else if (Effect.Spec.Def->GetGrantedTags().HasTag(CooldownTag))
			{
				CooldownTotal = Effect.GetDuration();
				CooldownStartTime = Effect.GetTimeRemaining(GetWorld()->GetTimeSeconds());
			}
		}
	}

	CooldownTotal -= SkillCoolTime_E;

	//if (HasAuthority())
		//TPT_LOG(LogTemp, Error, TEXT("AuraTotal: %.2f| AuraStartTime: %.2f| CooldownTotal: %.2f| CooldownStartTime: %.2f|"), SprintTotal, SprintStartTime, CooldownTotal, CooldownStartTime);

	float AuraPercent = 0;
	float CooldownPercent = 0;

	//percent 연산
	if (AuraTotal > 0.f)
	{
		AuraPercent = FMath::Clamp((AuraStartTime / AuraTotal), 0.f, 1.f);
	}

	if (AuraPercent <= 0.f)
	{
		CooldownPercent = FMath::Clamp(1.f - (CooldownStartTime / CooldownTotal), 0.f, 1.f);
	}
	else
	{
		CooldownPercent = 0.f;
	}

	OnAuraSkillUI.Broadcast(AuraPercent, CooldownPercent);
}
