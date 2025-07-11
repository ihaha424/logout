// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SzObjects/BaseObject.h"
#include "StaticObject.generated.h"

/**
 * 
 */
UCLASS()
class TPTOBJECTS_API AStaticObject : public ABaseObject
{
	GENERATED_BODY()


public:
	AStaticObject();

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "StaticObject", Replicated)
	bool bActived = false;
};
