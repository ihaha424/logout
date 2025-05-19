// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyPlayerUserWidget.h"
#include "PlayerHpBar.generated.h"

class UProgressBar;
/**
 * 
 */
UCLASS()
class THE_PHANTOM_TWINS_API UPlayerHpBar : public UMyPlayerUserWidget
{
	GENERATED_BODY()
	
public:
	UPlayerHpBar(const FObjectInitializer& ObjectInitializer);

	FORCEINLINE void SetMaxHp(float NewMaxHp) { MaxHp = NewMaxHp; }

protected:

	virtual void NativeConstruct() override;

	UPROPERTY()
	TObjectPtr<UProgressBar> HpProgressBar;

	UPROPERTY()
	float MaxHp;
};
