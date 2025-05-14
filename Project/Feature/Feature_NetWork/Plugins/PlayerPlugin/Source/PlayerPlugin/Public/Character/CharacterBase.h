// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interface/PlayerWidgetInterface.h"
#include "CharacterBase.generated.h"

class UPlayerStatComponent;
class UWidgetComponent;

UCLASS()
class PLAYERPLUGIN_API ACharacterBase : public ACharacter, public IPlayerWidgetInterface
{
	GENERATED_BODY()

public:
	
	ACharacterBase();

	virtual void PostInitializeComponents() override;

	void SetGroggy();

	UFUNCTION(BlueprintCallable, Category = "Stats")
	void TakeDamage(float Damage);

protected:

	// Stat Section
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Stat)
	TObjectPtr<UPlayerStatComponent> Stat;

	// UI Widget Section
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Widget)
	TObjectPtr<UWidgetComponent> GroggyWidget;

	virtual void SetupCharacterWidget(UMyPlayerUserWidget* UserWidget) override;
public:	


};
