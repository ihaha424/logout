// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MyPlayerUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class THE_PHANTOM_TWINS_API UMyPlayerUserWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	FORCEINLINE void SetOwningActor(AActor* NewOwner) { OwningActor = NewOwner; }

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Actor")
	TObjectPtr<AActor> OwningActor;
};
