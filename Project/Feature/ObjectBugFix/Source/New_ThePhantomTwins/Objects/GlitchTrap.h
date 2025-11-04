// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SzObjects/OverlapObject.h"
#include "GlitchTrap.generated.h"

/**
 * 
 */
UCLASS()
class NEW_THEPHANTOMTWINS_API AGlitchTrap : public AOverlapObject
{
	GENERATED_BODY()
	
public:
	AGlitchTrap();

public:
	// Activate 로직을 블루프린트에서 구현
	void ActivateMentalDamage();

	// Deactivate 로직을 블루프린트에서 구현
	void DeactivateMentalDamage(float DisableDuration);
    
	// 태그 변수 추가
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AGlitchTrap")
    FName ObjectTag;
};
