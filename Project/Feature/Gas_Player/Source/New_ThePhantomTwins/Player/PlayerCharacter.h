// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "InputActionValue.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "GenericTeamAgentInterface.h"
#include "PlayerCharacter.generated.h"

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

UCLASS()
class NEW_THEPHANTOMTWINS_API APlayerCharacter : public ACharacter, public IAbilitySystemInterface, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	APlayerCharacter();

	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;
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


	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);

	void InputPressed(int32 InputID);
	void InputReleased(int32 InputID);


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float WalkSpeed = 400.f;


public:
	// Ä«¸Þ¶ó
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> Camera;

protected:

	UPROPERTY()
	TObjectPtr<APS_Player> PS;

	// GAS
	UPROPERTY(EditAnywhere, Category = GAS)
	TObjectPtr<UAbilitySystemComponent> ASC;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAS")
	TSubclassOf<UGameplayEffect> InitAttributeSetEffect;

	UPROPERTY(EditAnywhere, Category = "GAS")
	TMap<FGameplayTag, TSubclassOf<UGameplayAbility>> PlayerAbilities;

	// ÀÎÇ²
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UDA_InputConfig* InputConfig;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> LookAction;

private:
};

