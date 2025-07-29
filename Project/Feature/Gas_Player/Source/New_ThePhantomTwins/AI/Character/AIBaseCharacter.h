// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "AIBaseCharacter.generated.h"

class UGameplayAbility;
class UAbilitySystemComponent;
class UAIBaseAttributeSet;
class ASplineActor;
class USphereComponent;
class UShapeComponent;
class UGameplayEffect;

UCLASS()
class NEW_THEPHANTOMTWINS_API AAIBaseCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AAIBaseCharacter();

	//~ Begin ACharacter interface
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void PostInitializeComponents() override;
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

	//~ Begin AI Control(Combat)
	UFUNCTION()
	void CombatRangeBeginOverlap(UPrimitiveComponent* OverlappedComp
		, AActor* OtherActor
		, UPrimitiveComponent* OtherComp
		, int32 OtherBodyIndex
		, bool bFromSweep
		, const FHitResult& SweepResult
	);

	void SetAttackCollision(bool bIsActive);
	UShapeComponent& GetAttackCollision() const;
	UFUNCTION()
	void AttackCollisionBeginOverlap(UPrimitiveComponent* OverlappedComp
		, AActor* OtherActor
		, UPrimitiveComponent* OtherComp
		, int32 OtherBodyIndex
		, bool bFromSweep
		, const FHitResult& SweepResult
	);
	UFUNCTION(BlueprintNativeEvent)
	void AttackCollisionEvent(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	void AttackCollisionEvent_Implementation(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {};
	//~ End AI Control(Combat)
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<UAbilitySystemComponent> AbilitySystem;

	UPROPERTY()
	TObjectPtr<UAIBaseAttributeSet> AttributeSet;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GAS")
	FGameplayTagContainer AIStateTags;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GAS")
	TMap<FGameplayTag, TSubclassOf<UGameplayAbility>>Abilities;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<USphereComponent> CombatRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GAS")
	TObjectPtr<UShapeComponent> AttackCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GAS")
	TSubclassOf<UGameplayEffect> DamageEffectClass;
};
