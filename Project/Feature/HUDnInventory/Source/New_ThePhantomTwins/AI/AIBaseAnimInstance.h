// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "AIBaseAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class NEW_THEPHANTOMTWINS_API UAIBaseAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	void SetLookAtTargetActor(AActor* LookAtTarget);






protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "LookAt")
	TObjectPtr<AActor> LookAtTargetActor;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "LookAt")
	FName LookAtBone;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "LookAt")
	float LookAtAlpha;
};
