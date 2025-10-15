// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BossSpawner.generated.h"

UCLASS()
class NEW_THEPHANTOMTWINS_API ABossSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	ABossSpawner();
	virtual void BeginPlay() override;

public:	
	UPROPERTY(EditAnywhere, Category = "Spawn")
	TSubclassOf<AActor> BossClass;
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Spawn")
	void SpawnBossOnce();

	UFUNCTION(BlueprintImplementableEvent, Category = "Spawn")
	void SpawnBoss();
};
