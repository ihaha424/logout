// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "InputActionValue.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "PlayerCharacter.generated.h"

class UGameplayEffect;
class APS_Player;
class UAbilitySystemComponent;
class UInputMappingContext;
class UCameraComponent;
class USpringArmComponent;
class UInputAction;

UCLASS()
class NEW_THEPHANTOMTWINS_API APlayerCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	APlayerCharacter();

	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	void SetupPlayerInputByTag();

	UFUNCTION()
	void OnPlayerDowned();

	UFUNCTION()
	void OnPlayerConfused();

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Interact(const FInputActionValue& Value);

protected:

	UPROPERTY()
	TObjectPtr<APS_Player> PS;

	// GAS
	UPROPERTY(EditAnywhere, Category = GAS)
	TObjectPtr<UAbilitySystemComponent> ASC;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAS")
	TSubclassOf<UGameplayEffect> InitAttributeSetEffect;

	// ÀÎÇ²
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> IMC;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> InteractAction;

	// Ä«¸Þ¶ó
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> Camera;
};

