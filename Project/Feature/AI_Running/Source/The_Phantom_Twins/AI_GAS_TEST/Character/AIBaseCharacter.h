// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "AIBaseCharacter.generated.h"


class UAbilitySystemComponent;
class UAIBaseAttributeSet;
class ASplineActor;

UCLASS()
class THE_PHANTOM_TWINS_API AAIBaseCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AAIBaseCharacter();

	//~ Begin ACharacter interface
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	//~ End ACharacter interface

	//~ Begin IAbilitySystemInterface interface & Additional GAS System
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UAIBaseAttributeSet* GetAIAttributeSet() const;
	//~ End IAbilitySystemInterface interface & Additional GAS System

	//~ Begin State Control
	void ApplyStun();
	void ResetToDefaultState();

	UFUNCTION(BlueprintCallable, Category = "AI")
	FString  GetCurrentAIStateAsString() const;
	//~ End State Control

	//~ Begin AI Control(Patrol)
	ASplineActor* GetBaseSplineActor() { return BaseSplineActor; }
	UPROPERTY(EditAnywhere, Category = "AI")
	TObjectPtr<ASplineActor> BaseSplineActor;
	//~ End AI Control(Patrol)

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UAbilitySystemComponent> AbilitySystem;

	UPROPERTY()
	TObjectPtr<UAIBaseAttributeSet> AttributeSet;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTagContainer AIStateTags;
};
