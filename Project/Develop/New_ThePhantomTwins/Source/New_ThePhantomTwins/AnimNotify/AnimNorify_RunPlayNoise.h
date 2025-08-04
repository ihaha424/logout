// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNorify_RunPlayNoise.generated.h"

/**
 * 
 */
UCLASS()
class NEW_THEPHANTOMTWINS_API UAnimNorify_RunPlayNoise : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sound")
	TObjectPtr<USoundBase> StepSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sound")
	float WalkNoise = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sound")
	float RunNoise = 1.f;
};