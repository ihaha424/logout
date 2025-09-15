// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Navigation/NavLinkProxy.h"
#include "GameplayTagContainer.h"
#include "GameplayTagNavLinkProxy.generated.h"

/**
 * 
 */
UCLASS()
class NEW_THEPHANTOMTWINS_API AGameplayTagNavLinkProxy : public ANavLinkProxy
{
	GENERATED_BODY()
	
public:
	AGameplayTagNavLinkProxy();

	virtual void BeginPlay() override;

	UFUNCTION()
	void HandleSmartLinkReached(AActor* MovingActor, const FVector& DestinationPoint);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GAS")
	FGameplayTag AbilityTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GAS")
	TObjectPtr<AActor> TargetActor;
};
