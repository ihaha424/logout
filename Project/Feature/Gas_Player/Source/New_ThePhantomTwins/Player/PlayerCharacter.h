// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "InputActionValue.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "SzInterface/Interact.h"
#include "GenericTeamAgentInterface.h"
#include "PlayerCharacter.generated.h"


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

	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_Controller() override;
	virtual void OnRep_PlayerState() override;
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	virtual FGenericTeamId GetGenericTeamId() const override;

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	void SetupPlayerInputByTag(UTPTEnhancedInputComponent* TPTInputComponent);
	UFUNCTION()
	void ExecuteAbilityByTag(FGameplayTag InputTag);
	UFUNCTION()
	void BindAttributeDelegates(const UPlayerAttributeSet* AttributeSet);

	UFUNCTION()
	void OnRecoveryCompleted();

	void SetWidgetVisibility(bool bNewVisibility);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float WalkSpeed = 400.f;

public:
	virtual bool CanInteract_Implementation(const APawn* Interactor, bool bIsDetected) override;
	virtual void OnInteractServer_Implementation(const APawn* Interactor) override;
	virtual void OnInteractClient_Implementation(const APawn* Interactor) override;
public:
	// 카메라
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> Camera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FocusTrace")
	TObjectPtr<UFocusTraceComponent> FocusTrace;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recovery")
	TObjectPtr<UWidgetComponent> InteractWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recovery")
	TObjectPtr<UWidgetComponent> RecoveryWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recovery")
	TSubclassOf<UUserWidget> RecoveryWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recovery")
	TSubclassOf<UUserWidget> InteractWidgetClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Recovery")
	FTimerHandle RecoveryTimerHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recovery")
	TObjectPtr<UGameplayEffect> RecoveryGE;

protected:

	UPROPERTY()
	TObjectPtr<APS_Player> PS;

	UPROPERTY()
	APC_Player* PlayerController;

	UPROPERTY()
	AHUD_PhantomTwins* PlayerHUD;

	// GAS
	UPROPERTY(EditAnywhere, Category = GAS)
	TObjectPtr<UAbilitySystemComponent> ASC;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAS")
	TSubclassOf<UGameplayEffect> InitAttributeSetEffect;

	UPROPERTY(EditAnywhere, Category = "GAS")
	TMap<FGameplayTag, TSubclassOf<UGameplayAbility>> PlayerAbilities;

	// 인풋
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UDA_InputConfig* InputConfig;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> LookAction;

	// 반경
	UPROPERTY(VisibleAnywhere)
	USphereComponent* WallSina;

	UPROPERTY(VisibleAnywhere)
	USphereComponent* WallRose;

	UPROPERTY(VisibleAnywhere)
	USphereComponent* WallMaria;

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
public:
	UFUNCTION()
	void PlayerHUDHPSet(int32 value);
	UFUNCTION()
	void PlayerHUDMentalSet(int32 value);
	UFUNCTION()
	void PlayerHUDStaminaSet(int32 value);
	UFUNCTION()
	void PlayerHUDCoreEnergySet(int32 value);

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);

	void InputPressed(int32 InputID);
	void InputPressedWithNum(int32 InputID, int32 Number);
	void InputReleased(int32 InputID);

	void OverlapRangeSetting();
	void MovementSetting();
	void CameraSetting();
	void OnBeginOverlap(EEnemyRange Range, AActor* OtherActor);
	void OnEndOverlap(EEnemyRange Range, AActor* OtherActor);
};
