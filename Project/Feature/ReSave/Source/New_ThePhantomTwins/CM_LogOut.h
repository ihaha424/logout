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
	void FullHP();

	UFUNCTION(exec)
	void SetMental(float mental);

	UFUNCTION(exec)
	void FullMental();

	UFUNCTION(exec)
	void SetRecovery(bool bFlag);

	UFUNCTION(exec)
	void GiveAllData();

	UFUNCTION(exec)
	void MoveToExit();

	UFUNCTION(exec)
	void StartST1();

	UFUNCTION(exec)
	void StartST2();

	UFUNCTION(exec)
	void ClearNow();

	UFUNCTION(exec)
	void GiveItems();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CheatCommand")
	FVector ExitPosition = FVector(0.f, 0.f, 300.f);
};
