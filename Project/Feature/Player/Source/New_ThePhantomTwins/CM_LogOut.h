// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CheatManager.h"
#include "CM_LogOut.generated.h"

/**
 * 
 */
UCLASS()
class NEW_THEPHANTOMTWINS_API UCM_LogOut : public UCheatManager
{
	GENERATED_BODY()

public:
	// À¯´ÖÀÇ Ä¸½¶ Ãâ·Â
	UFUNCTION(exec)
	void SetHP(float hp);

	UFUNCTION(exec)
	void SetMaxHP();

	UFUNCTION(exec)
	void SetRecovery(bool bFlag);
};
