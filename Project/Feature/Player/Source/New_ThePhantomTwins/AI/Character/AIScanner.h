// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/Character/AIBaseCharacter.h"
#include "Abilities/GameplayAbility.h"
#include "AIScanner.generated.h"

class ULightComponent;

USTRUCT()
struct FLightState
{
	GENERATED_BODY()
	float OriginalIntensity = 0.f;
	int32 OverlapCount = 0;
};


UCLASS()
class NEW_THEPHANTOMTWINS_API AAIScanner : public AAIBaseCharacter
{
	GENERATED_BODY()

public:
	virtual bool MatchingChaseActorType(AActor* OtherActor) const override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

protected:
	UFUNCTION()
	void UpdateNearbySpotLights();

protected:
	//~ Begin Light On/Off
	UPROPERTY()
	float LightAffectRadius = 2000.f;

	UPROPERTY()
	TMap<TWeakObjectPtr<ULightComponent>, FLightState> ManagedLights;
	//~ End Light On/Off

	//~ Begin Berserker Mode
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scanner")
	float BerserkerSpeed = 300;
	float AdditionalBerserkerSpeed = 0.f;
	UPROPERTY(EditAnywhere, Category = "Scanner")
	TSubclassOf<class UGameplayEffect> GameplayEffectClass;
	FActiveGameplayEffectHandle ActivcBerserkertHandle;
	UFUNCTION(BlueprintCallable, Category = "Scanner")
	void SetBerserkerMode(const FGameplayTag Tag, int32 TagCount);
	virtual void ResetDataForCombatState_Implementation() override;
	virtual void ResetDataForEscapeCombatState_Implementation() override;
	//~ End Berserker Mode
};
