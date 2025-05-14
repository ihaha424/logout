// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TestObject.generated.h"

class UWidgetComponent;
class UAIPerceptionStimuliSourceComponent;
class UStaticMeshComponent;

UCLASS()
class PLAYERPLUGIN_API ATestObject : public AActor
{
	GENERATED_BODY()
public:	
	ATestObject();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TObjectPtr<UWidgetComponent> Widget;

	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	//TObjectPtr<UAIPerceptionStimuliSourceComponent> StimuliSource;

protected:
	
public:	
	
};
