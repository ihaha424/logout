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
#include "FocusTraceComponent.h"
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
#include "UI/HUD/PlayerHUDWidget.h"
#include "Net/UnrealNetwork.h"
#include "Components/PostProcessComponent.h"

APlayerCharacter::APlayerCharacter()
{
	ASC = nullptr;
	bReplicates = true;
	SetReplicates(true);

	MovementSetting();
	CameraSetting();
	OverlapRangeSetting();

	FocusTrace = CreateDefaultSubobject<UFocusTraceComponent>(TEXT("FocusTrace"));
	FocusTrace->SetIsReplicated(true);

	InteractWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractWidget"));
	InteractWidget->SetupAttachment(GetMesh());

	DownedWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("DownedWidget"));
	DownedWidget->SetupAttachment(GetMesh());
	DownedWidget->SetRelativeLocation(FVector(0, 0, 0));

	PostProcessComponent = CreateDefaultSubobject<UPostProcessComponent>(TEXT("Vignette"));
	PostProcessComponent->SetupAttachment(RootComponent);
	PostProcessComponent->bUnbound = false;
	PostProcessComponent->Priority = 1.f;
}

void APlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	PS = GetPlayerState<APS_Player>();
	NULLCHECK_RETURN_LOG(PS, PlayerLog, Error, );
	PS->SetIdentifyCharacterData();
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
	PS->SetIdentifyCharacterData();
	SetMeshByCharacterType(PS);

	ASC = PS->GetAbilitySystemComponent();
	ASC->InitAbilityActorInfo(PS, this);
	NULLCHECK_RETURN_LOG(ASC, PlayerLog, Error, );
	const UPlayerAttributeSet* AttributeSet = ASC->GetSet<UPlayerAttributeSet>();
	NULLCHECK_RETURN_LOG(AttributeSet, PlayerLog, Error, );
	BindAttributeDelegates(AttributeSet);

	InitHUDWidget(AttributeSet);
	UPlayerAnimInstance* AnimInstance = Cast<UPlayerAnimInstance>(GetMesh()->GetAnimInstance());
	AnimInstance->InitializeWithAbilitySystem(ASC);
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	NULLCHECK_RETURN_LOG(InteractWidget, PlayerLog, Error, );

	UUserWidget* Interact = CreateWidget(GetWorld(), InteractWidgetClass);
	InteractWidget->SetWidget(Interact);
	InteractWidget->GetUserWidgetObject()->SetVisibility(ESlateVisibility::Hidden);

	UUserWidget* Down = CreateWidget(GetWorld(), DownWidgetClass);
	DownedWidget->SetWidget(Down);
	DownedWidget->GetUserWidgetObject()->SetVisibility(ESlateVisibility::Hidden);

	FocusTrace->SetIsReplicated(true);

	NULLCHECK_RETURN_LOG(VignetteMaterial, PlayerLog, Error, );
	VignetteMID = UMaterialInstanceDynamic::Create(VignetteMaterial, this);
	FWeightedBlendable Blendable;
	Blendable.Object = VignetteMID;
	Blendable.Weight = 0.0f;

	PostProcessComponent->Settings.WeightedBlendables.Array.Add(Blendable);

	if (IsLocallyControlled())
	{
		PlayerController->RegisterWidget(TEXT("RecoveryGauge"), CreateWidget<UUserWidget>(GetWorld(), RecoveryWidgetClass));
		PlayerController->RegisterWidget(TEXT("WASD"), CreateWidget<UUserWidget>(GetWorld(), KeyWidgetClass));
	}
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (ASC)
	{
		bool bIsDownedTag = ASC->HasMatchingGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_State_Downed);
		if (bIsDownedTag)
		{
			DownedWidget->GetUserWidgetObject()->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			DownedWidget->GetUserWidgetObject()->SetVisibility(ESlateVisibility::Hidden);
		}
	}

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
		
		// 클라에선 FocusTrace 세팅(시각효과용)
		FocusTrace->SetStart(WorldLocation);
		FocusTrace->SetDirection(WorldDirection);
		FocusTrace->SetCollisionType(ECC_WorldDynamic);

		// 서버 RPC 호출에 위치 + 회전 같이 넘기기
		C2S_SetFocusTrace(CameraLocation, CameraRotation);
	}
}

void APlayerCharacter::C2S_SetFocusTrace_Implementation(const FVector& CameraLocation, const FRotator& CameraRotation)
{
	if (HasAuthority() && FocusTrace)
	{
		FocusTrace->SetStart(CameraLocation);

		FVector Direction = CameraRotation.Vector(); // 카메라 회전 기반 전방 방향 벡터를 다시 구함
		FocusTrace->SetDirection(Direction);

		FocusTrace->SetCollisionType(ECC_WorldDynamic);
	}
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// 로컬에서만 일어남.
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	UTPTEnhancedInputComponent* TPTInput = CastChecked<UTPTEnhancedInputComponent>(PlayerInputComponent);
	check(TPTInput);
	TPTInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ThisClass::Move);
	TPTInput->BindAction(LookAction, ETriggerEvent::Triggered, this, &ThisClass::Look);

	NULLCHECK_RETURN_LOG(PlayerController, PlayerLog, Error, );

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
		TPTInput->BindActionByTag(InputConfig, FTPTGameplayTags::Get().TPTGameplay_InputTag_Player_ItemSlot_1st, ETriggerEvent::Started, this, &ThisClass::InputPressedWithNum, 1);
		TPTInput->BindActionByTag(InputConfig, FTPTGameplayTags::Get().TPTGameplay_InputTag_Player_ItemSlot_2nd, ETriggerEvent::Started, this, &ThisClass::InputPressedWithNum, 2);
		TPTInput->BindActionByTag(InputConfig, FTPTGameplayTags::Get().TPTGameplay_InputTag_Player_ItemSlot_3rd, ETriggerEvent::Started, this, &ThisClass::InputPressedWithNum, 3);
		TPTInput->BindActionByTag(InputConfig, FTPTGameplayTags::Get().TPTGameplay_InputTag_Player_ItemSlot_4th, ETriggerEvent::Started, this, &ThisClass::InputPressedWithNum, 4);
		TPTInput->BindActionByTag(InputConfig, FTPTGameplayTags::Get().TPTGameplay_InputTag_Player_ItemSlot_5th, ETriggerEvent::Started, this, &ThisClass::InputPressedWithNum, 5);
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
	if (!HasAuthority())
	{
		C2S_InputReleased(InputID);
	}
	else if (Spec->IsActive())
	{
		ASC->AbilitySpecInputReleased(*Spec);
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

void APlayerCharacter::InputPressedWithNum(int32 InputID, int32 Number)
{
	FGameplayTag EventTag = FTPTGameplayTags::Get().TPTGameplay_Event_Character_UseItemSlot;
	FGameplayEventData Payload;
	Payload.EventTag = EventTag;
	Payload.Instigator = this;
	Payload.EventMagnitude = static_cast<float>(Number);

	ASC->HandleGameplayEvent(EventTag, &Payload);
}

void APlayerCharacter::BindAttributeDelegates(const UPlayerAttributeSet* AttributeSet)
{
	AttributeSet->OnPlayerLowHP.AddDynamic(this, &ThisClass::ExecuteAbilityByTag);
	AttributeSet->OnPlayerDowned.AddDynamic(this, &ThisClass::ExecuteAbilityByTag);
	AttributeSet->OnPlayerConfused1st.AddDynamic(this, &ThisClass::ExecuteAbilityByTag);
	AttributeSet->OnPlayerConfused2nd.AddDynamic(this, &ThisClass::ExecuteAbilityByTag);
	AttributeSet->OnPlayerConfused3rd.AddDynamic(this, &ThisClass::ExecuteAbilityByTag);
	AttributeSet->OnPlayerUseSkill.AddDynamic(this, &ThisClass::ExecuteAbilityByTag);
	AttributeSet->OnMentalPointNotMax.AddDynamic(this, &ThisClass::ExecuteAbilityByTag);

	if (IsLocallyControlled())
	{
		AttributeSet->OnChangedHP.AddDynamic(this, &ThisClass::PlayerHUDHPSet);
		AttributeSet->OnChangedMentalPoint.AddDynamic(this, &ThisClass::PlayerHUDMentalSet);
		AttributeSet->OnChangedStamina.AddDynamic(this, &ThisClass::PlayerHUDStaminaSet);
		AttributeSet->OnChangedCoreEnergy.AddDynamic(this, &ThisClass::PlayerHUDCoreEnergySet);
	}

	if (HasAuthority())
	{
		WallSina->OnComponentBeginOverlap.AddDynamic(this, &APlayerCharacter::OnBeginOverlapWall);
		WallSina->OnComponentEndOverlap.AddDynamic(this, &APlayerCharacter::OnEndOverlapWall);
		WallRose->OnComponentBeginOverlap.AddDynamic(this, &APlayerCharacter::OnBeginOverlapWall);
		WallRose->OnComponentEndOverlap.AddDynamic(this, &APlayerCharacter::OnEndOverlapWall);
		WallMaria->OnComponentBeginOverlap.AddDynamic(this, &APlayerCharacter::OnBeginOverlapWall);
		WallMaria->OnComponentEndOverlap.AddDynamic(this, &APlayerCharacter::OnEndOverlapWall);
	}
}

void APlayerCharacter::ExecuteAbilityByTag(FGameplayTag InputTag)
{
	FGameplayAbilitySpec* TagID = ASC->FindAbilitySpecFromInputID(static_cast<int32>(FTPTGameplayTags::Get().TagMap[InputTag]));
	NULLCHECK_RETURN_LOG(TagID, PlayerLog, Warning, );
	ASC->TryActivateAbilitiesByTag(FGameplayTagContainer(InputTag));
}

void APlayerCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APlayerCharacter, RecoveryPercent);
	DOREPLIFETIME(APlayerCharacter, CurrentWallRange);
}

bool APlayerCharacter::CanInteract_Implementation(const APawn* Interactor, bool bIsDetected)
{
	NULLCHECK_RETURN_LOG(ASC, PlayerLog, Error, false);
	bool bIsTag = ASC->HasMatchingGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_State_Downed);

	if (bIsTag && bIsDetected)
	{
		InteractWidget->GetUserWidgetObject()->SetVisibility(ESlateVisibility::Visible);
		return true;
	}
	InteractWidget->GetUserWidgetObject()->SetVisibility(ESlateVisibility::Hidden);
	return false;
}

void APlayerCharacter::OnInteractServer_Implementation(const APawn* Interactor)
{

	FTimerDelegate TimerDel;
	TimerDel.BindLambda([this, Interactor]()
		{
			if (GetWorld()->GetTimerManager().IsTimerActive(RecoveryTimerHandle))
			{
				float Elapsed = GetWorld()->GetTimerManager().GetTimerElapsed(RecoveryTimerHandle);
				RecoveryPercent = Elapsed / RecoveryTime;
				APawn* interactPawn = const_cast<APawn*>(Interactor);
				APlayerCharacter* interactCharacter = Cast<APlayerCharacter>(interactPawn);
				interactCharacter->RecoveryPercent = RecoveryPercent;
			}
		});

	GetWorld()->GetTimerManager().SetTimer(
		TempHandle,
		TimerDel,
		0.02f,
		true
	);

	GetWorld()->GetTimerManager().SetTimer(
		RecoveryTimerHandle,
		this,
		&APlayerCharacter::OnRecoveryCompleted,
		RecoveryTime,
		false
	);
}

void APlayerCharacter::OnInteractClient_Implementation(const APawn* Interactor)
{
	APC_Player* PC = APC_Player::GetLocalPlayerController(Interactor->GetController());
	if (!ASC->HasMatchingGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_State_Downed))
		return;
	PC->SetWidget(TEXT("RecoveryGauge"), true, EMessageTargetType::Multicast);
}

void APlayerCharacter::OnRecoveryCompleted()
{
	NULLCHECK_RETURN_LOG(PS, PlayerLog, Error, );
	NULLCHECK_RETURN_LOG(ASC, PlayerLog, Error, );
	PS->SetRecovery(true);

	FGameplayTag DownedTag = FTPTGameplayTags::Get().TPTGameplay_Character_State_Downed;
	FGameplayTag ConfusedTag = FTPTGameplayTags::Get().TPTGameplay_Character_State_Confused3rd;
	FGameplayTag LowHPTag = FTPTGameplayTags::Get().TPTGameplay_Character_State_LowHP;

	int32 DownedTagCount = ASC->GetTagCount(DownedTag);
	for (int32 i = 0; i < DownedTagCount; ++i)
	{
		ASC->RemoveLooseGameplayTag(DownedTag);
	}

	int32 LowHPTagCount = ASC->GetTagCount(LowHPTag);
	for (int32 i = 0; i < LowHPTagCount; ++i)
	{
		ASC->RemoveReplicatedLooseGameplayTag(LowHPTag);
		ASC->RemoveLooseGameplayTag(LowHPTag);
	}

	if (ASC->HasMatchingGameplayTag(ConfusedTag))
	{
		ASC->RemoveLooseGameplayTag(ConfusedTag);
	}

	GetWorld()->GetTimerManager().ClearTimer(RecoveryTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(TempHandle);
	InteractWidget->GetUserWidgetObject()->SetVisibility(ESlateVisibility::Hidden);
	DownedWidget->GetUserWidgetObject()->SetVisibility(ESlateVisibility::Hidden);

	APC_Player* PC = APC_Player::GetLocalPlayerController(this);

	PC->SetWidget(TEXT("RecoveryGauge"), false, EMessageTargetType::Multicast);
	PC->SetWidget(TEXT("WASD"), false, EMessageTargetType::LocalClient);

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
}

void APlayerCharacter::InitHUDWidget(const UPlayerAttributeSet* AttributeSet)
{// AttributeSet이 없으면 바로 반환
	if (!AttributeSet) return;

	if (!IsLocallyControlled())
	{
		// 로그로 어느 객체에서 호출됐는지 안내
		TPT_LOG(HUDLog, Warning, TEXT("InitHUDWidget: Not locally controlled, skipping widget creation (Actor: %s)"), *GetName());
		return;
	}

	// PlayerHUDWidget이 아직 생성되지 않았다면 생성
	if (!PlayerHUDWidget)
	{
		APlayerController* PC = Cast<APlayerController>(GetController());

		if (PC && PlayerHUDWidgetClass)
		{
			PlayerController->RegisterWidget(TEXT("PlayerHUDWidget"), CreateWidget<UPlayerHUDWidget>(GetWorld(), PlayerHUDWidgetClass));
			PlayerController->SetWidget(TEXT("PlayerHUDWidget"), true, EMessageTargetType::LocalClient);
			PlayerHUDWidget = Cast<UPlayerHUDWidget>(PlayerController->GetWidget(TEXT("PlayerHUDWidget")));
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
	int32 CoreEnergy = AttributeSet->GetMaxCoreEnergy();

	PlayerHUDWidget->InitializeWidgets(HP, Mental, Stamina, CoreEnergy);

	//PS에 있는 Inventory의 SetPlayerHUDWidget 호출
	PS->InventoryComp->SetPlayerHUDWidget(PlayerHUDWidget);
}

void APlayerCharacter::PlayerHUDHPSet(int32 value)
{
	NULLCHECK_RETURN_LOG(PlayerHUDWidget, HUDLog, Error, );
	PlayerHUDWidget->UpdateHP(value);

	//PlayerHUD->UpdateClearItem();// 데이터조각(같이공유하는거)
	//// 태그이용
	//PlayerHUD->SetActiveSkillIcon();
	//PlayerHUD->SetPassiveSkillIcon();
	//PlayerHUD->SetCharPortrait();// 초상화
}

void APlayerCharacter::PlayerHUDMentalSet(int32 value)
{
	NULLCHECK_RETURN_LOG(PlayerHUDWidget, HUDLog, Error, );
	PlayerHUDWidget->UpdateMental(value);
}

void APlayerCharacter::PlayerHUDStaminaSet(int32 value)
{
	NULLCHECK_RETURN_LOG(PlayerHUDWidget, HUDLog, Error, );
	PlayerHUDWidget->UpdateStamina(value);
}

void APlayerCharacter::PlayerHUDCoreEnergySet(int32 value)
{
	NULLCHECK_RETURN_LOG(PlayerHUDWidget, HUDLog, Error, );
	PlayerHUDWidget->UpdateCoreEnergy(value);
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
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	GetCharacterMovement()->MaxWalkSpeedCrouched = 80.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 600.f;
}

void APlayerCharacter::CameraSetting()
{
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->bEnableCameraLag = true;
	SpringArm->TargetArmLength = 150.0f;

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

	NULLCHECK_RETURN_LOG(AIASC, PlayerLog, Log, );
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

void APlayerCharacter::OnEndOverlap(EEnemyRange Range, AActor* OtherActor)
{
	UAbilitySystemComponent* AIASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OtherActor);
	NULLCHECK_RETURN_LOG(AIASC, PlayerLog, Log, );
	if (AIASC->HasMatchingGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_Identifier_AI))
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

	if (ToPlay)
	{
		WallAudioComponent = UGameplayStatics::SpawnSoundAttached(ToPlay, GetRootComponent());
	}
}

void APlayerCharacter::OnRep_CurrentWallRange()
{
	UpdateWallSound();
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