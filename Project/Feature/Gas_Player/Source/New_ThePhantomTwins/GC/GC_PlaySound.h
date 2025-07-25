// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Actor.h"
#include "GC_PlaySound.generated.h"

/**
 * 
 */
UCLASS()
class NEW_THEPHANTOMTWINS_API AGC_PlaySound : public AGameplayCueNotify_Actor
{
	GENERATED_BODY()
public:
    AGC_PlaySound();

    // OnActive에서 ASC의 태그를 판별하여 각각의 사운드 재생
    virtual bool OnActive_Implementation(AActor* MyTarget,const FGameplayCueParameters& Parameters) override;
    virtual bool OnRemove_Implementation(AActor* Target, const FGameplayCueParameters& Parameters) override;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SFX")
    USoundBase* SoundConfused1st;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SFX")
    USoundBase* SoundConfused2nd;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SFX")
    USoundBase* SoundConfused3rd;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SFX")
    USoundBase* PlayTarget;

    UPROPERTY()
    UAudioComponent* ConfusedAudioComponent = nullptr;
};
