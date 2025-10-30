// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "Data/MapType.h"
#include "AnimNotify_WalkSound.generated.h"


USTRUCT(BlueprintType)
struct NEW_THEPHANTOMTWINS_API FNoiseType
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<USoundBase> stepSound;
	/*
		"EnemyRun"
		"EnemyWalk"
		"PlayerRun"
		"PlayerWalk"
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Type;
};

UCLASS()
class NEW_THEPHANTOMTWINS_API UAnimNotify_WalkSound : public UAnimNotify
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	TMap<EMapType, FNoiseType> StepSoundByMap;

	FString GetNotifyName_Implementation() const;
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
