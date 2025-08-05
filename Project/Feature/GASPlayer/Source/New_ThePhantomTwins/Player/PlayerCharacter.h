// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "InputActionValue.h"
#include "GameFramework/Character.h"
#include "GameplayTagContainer.h"
#include "SzInterface/Interact.h"
#include "GenericTeamAgentInterface.h"
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

UCLASS()
class NEW_THEPHANTOMTWINS_API APlayerCharacter : public ACharacter, public IAbilitySystemInterface, public IGenericTeamAgentInterface, public IInteract
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

public:
	// 플레이어 캐릭터 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float WalkSpeed = 150.f;
	// 리커버리 관련 변수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recovery")
	FTimerHandle RecoveryTimerHandle;
	FTimerHandle TempHandle;;
	// 다운 인디케이터
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recovery")
	TObjectPtr<UWidgetComponent> DownedWidget;
	
	// Low HP Post Process Vignette 관련 변수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PostProcess")
	TObjectPtr<UPostProcessComponent> PostProcessComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PostProcess")
	TObjectPtr<UMaterialInterface> VignetteMaterial;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PostProcess")
	TObjectPtr<UMaterialInstanceDynamic> VignetteMID;

public:
	// 위젯 설정
	void InitHUDWidget(const UPlayerAttributeSet* AttributeSet);
	UFUNCTION()
	void PlayerHUDHPSet(int32 value);
	UFUNCTION()
	void PlayerHUDMentalSet(int32 value);
	UFUNCTION()
	void PlayerHUDStaminaSet(int32 value);
	UFUNCTION()
	void PlayerHUDCoreEnergySet(int32 value);
protected:
	// 플레이어 인풋 바인딩
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	UFUNCTION()
	void SetupPlayerInputByTag(UTPTEnhancedInputComponent* TPTInputComponent);
	UFUNCTION()
	void ExecuteAbilityByTag(FGameplayTag InputTag);
	UFUNCTION()
	void BindAttributeDelegates(const UPlayerAttributeSet* AttributeSet);

	// 플레이어 상태변경
	UFUNCTION()
	void OnRecoveryCompleted();

	// 인풋 처리
	void InputPressed(int32 InputID);
	void InputPressedWithNum(int32 InputID, int32 Number);
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
	void CameraSetting();
	void OverlapRangeSetting();

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

protected:

	UPROPERTY()
	TObjectPtr<APS_Player> PS;
	UPROPERTY()
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recovery")
	float RecoveryTime = 3.0f;
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite)
	float RecoveryPercent = 0.0f;

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
};
