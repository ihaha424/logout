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
	UFUNCTION(BlueprintCallable, Category = "AI")
	void  ResetDataForState(const FGameplayTag Tag, int32 TagCount);
	//~ End State Control

	//~ Begin AI Control(Patrol)
	ASplineActor* GetBaseSplineActor() { return BaseSplineActor; }
	UPROPERTY(EditAnywhere, Category = "AI")
	TObjectPtr<ASplineActor> BaseSplineActor;
	//~ End AI Control(Patrol)

	//~ Begin AI Control(Combat)
	TArray<AActor*> CombatRangeInActor;
	FTimerHandle CombatRangeInActorTimerHandle;
	virtual bool MatchingChaseActorType(AActor* OtherActor) const;
	UFUNCTION()
	void CombatRangeBeginOverlap(UPrimitiveComponent* OverlappedComp
		, AActor* OtherActor
		, UPrimitiveComponent* OtherComp
		, int32 OtherBodyIndex
		, bool bFromSweep
		, const FHitResult& SweepResult
	);
	UFUNCTION()
	void CombatRangeEndOverlap(UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);
	void CheckCombatRangeInActor();
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
	void ExcuteChaseActorGA(AActor* TargetActor);
	void CancleChaseActorGA();
	//~ End AI Control(Combat)

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<UAbilitySystemComponent> AbilitySystem;

	//~ Begin AI Attribute
	UPROPERTY()
	TObjectPtr<UAIBaseAttributeSet> AttributeSet;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Attribute")
	float ChaseMentalAttackValue = 10;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Attribute")
	float AttackValue = 10;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Attribute")
	float MoveSpeed = 200;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI Attribute")
	float ChaseSpeed = 300;
	//~ End AI Attribute

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

protected:
	//~ Begin State Control
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI")
	void ResetDataForStunState();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI")
	void ResetDataForDefaultState();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI")
	void ResetDataForSuspicionState();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI")
	void ResetDataForCombatState();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI")
	void ResetDataForEscapeStunState();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI")
	void ResetDataForEscapeDefaultState();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI")
	void ResetDataForEscapeSuspicionState();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AI")
	void ResetDataForEscapeCombatState();

	void ResetDataForDefaultState_Implementation();
	void ResetDataForSuspicionState_Implementation();
	void ResetDataForStunState_Implementation();
	void ResetDataForCombatState_Implementation();
	void ResetDataForEscapeDefaultState_Implementation();
	void ResetDataForEscapeSuspicionState_Implementation();
	void ResetDataForEscapeStunState_Implementation();
	void ResetDataForEscapeCombatState_Implementation();
	//~ End State Control
};
