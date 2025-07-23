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

	// 게이지 시작
	UFUNCTION()
	void OnRecoveryCompelete();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recovery")
	bool bIsRecovery = false;

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
	APlayerController* PlayerController;

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
	void InputReleased(int32 InputID);
};

