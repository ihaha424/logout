// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/Controller/AIBaseController.h"
#include "AIScannerController.generated.h"

/**
 * 
 */
UCLASS()
class NEW_THEPHANTOMTWINS_API AAIScannerController : public AAIBaseController
{
	GENERATED_BODY()
	
public:
	AAIScannerController();

	//~ Begin AIBaseController
	virtual bool IsTargetActor(UAbilitySystemComponent* ASC) override;
	//~ End AIBaseController
};
