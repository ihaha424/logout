// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "InputActionValue.h"
#include "GameFramework/Character.h"
#include "GameplayTagContainer.h"
#include "SzInterface/Interact.h"
#include "SzInterface/Holding.h"
#include "GenericTeamAgentInterface.h"
#include "OutGame/HubMap/GS_HubMap.h"
#include "PlayerCharacter.generated.h"

class UPlayerHUDWidget;
class USphereComponent;
class APC_Player;
class AHUD_PhantomTwins;
class UPlayerAttributeSet;
class UGameplayAbility;
class UTPTEnhancedInputComponent;
class UDA_InputConfig;
class UGameplayEffect;
class APS_Player;
class UAbilitySystemComponent;
class UInputMappingContext;
class UCameraComponent;
class USpringArmComponent;
class UInputAction;
class UFocusTraceComponent;
class UWidgetComponent;
class UPostProcessComponent;

UENUM(BlueprintType)
enum class EEnemyRange : uint8
{
	None UMETA(DisplayName = "None"),
	WallSina UMETA(DisplayName = "WallSina"),
	WallRose UMETA(DisplayName = "WallRose"),
	WallMaria UMETA(DisplayName = "WallMaria")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSprintSkillUI, float, SprintPercent, float, CooldownPercent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAuraSkillUI, float, AuraPercent, float, CooldownPercent);

UCLASS()
class NEW_THEPHANTOMTWINS_API APlayerCharacter : public ACharacter, public IAbilitySystemInterface, public IGenericTeamAgentInterface, public IInteract, public IHolding
{
	GENERATED_BODY()

public:
	APlayerCharacter();

	virtual FGenericTeamId GetGenericTeamId() const override;
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;
	UPlayerHUDWidget* GetPlayerHUDWidget() const { return PlayerHUDWidget; }
	UFocusTraceComponent* GetFocusTrace() const { return FocusTrace; }
	USpringArmComponent* GetSpringArm() const { return SpringArm; }
	UCameraComponent* GetCamera() const { return Camera; }

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_Controller() override;
	virtual void OnRep_PlayerState() override;

	virtual bool CanInteract_Implementation(const APawn* Interactor, bool bIsDetected) override;
	virtual void OnInteractServer_Implementation(const APawn* Interactor) override;
	virtual void OnInteractClient_Implementation(const APawn* Interactor) override;

	virtual float GetTime_Implementation() override;
	virtual void CalculateGaugePercent_Implementation(float Elapsed) override;
	virtual void SetHoldingGaugeUI_Implementation(const APawn* Interactor, bool bVisible) override;

	UFUNCTION(BlueprintImplementableEvent)
	void OnRep_RecoveryPercent();

public:
	// 플레이어 캐릭터 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float WalkSpeed = 150.f;

	// 다운 인디케이터
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recovery")
	TObjectPtr<UWidgetComponent> DownedWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BoxCollision")
	TObjectPtr<class UBoxComponent> BoxComp;

	// 리커버리 관련 변수
	UPROPERTY(ReplicatedUsing = OnRep_RecoveryPercent, VisibleAnywhere, BlueprintReadWrite)
	float RecoveryPercent = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recovery")
	float RecoveryTime = 5.0f;

	// Low HP Post Process Vignette 관련 변수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PostProcess")
	TObjectPtr<UPostProcessComponent> PostProcessComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PostProcess")
	TObjectPtr<UMaterialInterface> VignetteMaterial;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PostProcess")
	TObjectPtr<UMaterialInstanceDynamic> VignetteMID;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item")
	TObjectPtr<class UHeldItemComponent> HeldItemComponent;

public:
	// 위젯 설정
	void InitHUDWidget(const UPlayerAttributeSet* AttributeSet);

	UFUNCTION()
	void PlayerHUDStaminaSet(int32 value);
	UFUNCTION()
	void HidePlayerHUDStaminaSet(int32 value);
	UFUNCTION()
	void PlayerHUDCoreEnergySet(int32 value);

	FORCEINLINE APS_Player* GetPS() const { return PS.Get(); }

	UFUNCTION(BlueprintCallable)
	void ExecuteAbilityByTag(FGameplayTag InputTag);
	UFUNCTION(BlueprintCallable)
	void GivePassiveSkillBySkillType(ESkillType Type);

	// Player Skill Cool Gauge Func
	UFUNCTION(BlueprintCallable)
	void UpdateSprintCooldownCount();
	UFUNCTION(BlueprintCallable)
	void UpdateAuraCooldownCount();

	UPROPERTY(BlueprintAssignable, Category = "Character Skill")
	FSprintSkillUI OnSprintSkillUI;
	UPROPERTY(BlueprintAssignable, Category = "Character Skill")
	FAuraSkillUI OnAuraSkillUI;

protected:
	// 플레이어 인풋 바인딩
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	UFUNCTION()
	void SetupPlayerInputByTag(UTPTEnhancedInputComponent* TPTInputComponent);
	UFUNCTION()
	void BindAttributeDelegates(const UPlayerAttributeSet* AttributeSet);
	UFUNCTION()
	void OnTagChanged(const FGameplayTag InputTag, int32 Count);

	// 플레이어 상태변경
	UFUNCTION()
	void OnRecoveryCompleted();

	// 인풋 처리
	void InputPressed(int32 InputID);
	void InputSKillPressed(int32 InputID, int32 Number);
	void InputPressedWithNum(int32 InputID, int32 Number);
	void InputMouseWheelUp(const FInputActionValue& Value);
	void InputMouseWheelDown(const FInputActionValue& Value);
	UFUNCTION(BlueprintCallable)
	void InputESC(const FInputActionValue& Value);
	void InputPressedUseItem(int32 InputID);
	void InputReleased(int32 InputID);

	UFUNCTION(Server, Reliable)
	void C2S_InputReleased(const int32 InputID);
	void C2S_InputReleased_Implementation(const int32 InputID);

	UFUNCTION(Server, Reliable)
	void C2S_SetFocusTrace(const FVector& CameraLocation, const FRotator& CameraRotation);
	void C2S_SetFocusTrace_Implementation(const FVector& CameraLocation, const FRotator& CameraRotation);

	// 기본 움직임 및 카메라 회전
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);

	// 플레이어 초기 세팅.
	void MovementSetting();
	UFUNCTION()
	void SpeedSetting(int32 Speed);
	void CameraSetting();
	void OverlapRangeSetting();
	UFUNCTION(BlueprintImplementableEvent)
	void SetMeshByCharacterType(APS_Player* MyPS);
	UFUNCTION(BlueprintImplementableEvent)
	void SetSelectSkill(APS_Player* MyPS);

	// 플레이어 반경 오버랩 처리
	UFUNCTION()
	void OnBeginOverlapWall(UPrimitiveComponent* Comp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnEndOverlapWall(UPrimitiveComponent* Comp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	void OnBeginOverlap(EEnemyRange Range, AActor* OtherActor);
	void OnEndOverlap(EEnemyRange Range, AActor* OtherActor);
	void SetNearestEnemyRange();
	void UpdateWallSound();
	UFUNCTION()
	void OnRep_CurrentWallRange();


private:
	UFUNCTION(BlueprintCallable, Category="Item")
	void RemoveHeldItemMesh();

	UFUNCTION(Server, Reliable)
	void C2S_RemoveHeldItemMesh();
	void C2S_RemoveHeldItemMesh_Implementation();

	UFUNCTION(NetMulticast, Reliable)
	void S2A_RemoveHeldItemMesh();
	void S2A_RemoveHeldItemMesh_Implementation();

	UFUNCTION(NetMulticast, Reliable)
	void S2A_OnDownedWidget(bool Visible);
	void S2A_OnDownedWidget_Implementation(bool Visible);

protected:
	UPROPERTY()
	TObjectPtr<APS_Player> PS;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "GAS")
	APC_Player* PlayerController;

	// GAS
	UPROPERTY(EditAnywhere, Category = "GAS")
	TObjectPtr<UAbilitySystemComponent> ASC;
	UPROPERTY(EditAnywhere, Category = "GAS")
	TMap<FGameplayTag, TSubclassOf<UGameplayAbility>> PlayerAbilities;

	// 이펙트
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAS")
	TSubclassOf<UGameplayEffect> InitAttributeSetEffect;
	UPROPERTY(EditAnywhere, Category = "Recovery")
	TSubclassOf<UGameplayEffect> RecoveryGE;

	// 인풋
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UDA_InputConfig* InputConfig;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> LookAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> MouseWheelUpAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> MouseWheelDownAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> ESC;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
	int32 SelectedSlotNumber = 0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
	int32 MaxSlotNumber = 5;

	// 카메라
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> SpringArm;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> Camera;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FocusTrace")
	TObjectPtr<UFocusTraceComponent> FocusTrace;

	// 스텟 위젯
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UPlayerHUDWidget> PlayerHUDWidgetClass;
	UPROPERTY()
	TObjectPtr<UPlayerHUDWidget> PlayerHUDWidget;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> CannotUseItemWidgetClass;
	
	// 상호작용 위젯
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recovery")
	TObjectPtr<UWidgetComponent> InteractWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recovery")
	TSubclassOf<UUserWidget> RecoveryWidgetClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recovery")
	TSubclassOf<UUserWidget> KeyWidgetClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recovery")
	TSubclassOf<UUserWidget> InteractWidgetClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recovery")
	TSubclassOf<UUserWidget> DownWidgetClass;

	// 재시작용 위젯
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	TSubclassOf<UUserWidget> GameOverWidgetClass;
	// 로딩용 위젯
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	TSubclassOf<UUserWidget> LoadingWidgetClass;
	// ESC용 위젯
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	TSubclassOf<UUserWidget> ESCWidgetClass;
	bool bIsShowingESC = false;
	// 게임 중지용 위젯
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	TSubclassOf<UUserWidget> GameStopWidgetClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	TSubclassOf<UUserWidget> ResumeCountWidgetClass;

	// 반경
	UPROPERTY(VisibleAnywhere)
	USphereComponent* WallSina;
	UPROPERTY(VisibleAnywhere)
	USphereComponent* WallRose;
	UPROPERTY(VisibleAnywhere)
	USphereComponent* WallMaria;

	// 사운드
	UPROPERTY(EditAnywhere, Category = Sound)
	USoundBase* WallSinaSound;
	UPROPERTY(EditAnywhere, Category = Sound)
	USoundBase* WallRoseSound;
	UPROPERTY(EditAnywhere, Category = Sound)
	USoundBase* WallMariaSound;
	UPROPERTY()
	UAudioComponent* WallAudioComponent = nullptr;
	UPROPERTY()
	TMap<AActor*, EEnemyRange> EnemyRangeMap;
	UPROPERTY(ReplicatedUsing = OnRep_CurrentWallRange)
	EEnemyRange CurrentWallRange = EEnemyRange::None;

	
private:
	UPROPERTY()
	FTimerHandle VisibleInventoryTimerHandle;
	FTimerHandle VisibleStaminaTimerHandle;

};
