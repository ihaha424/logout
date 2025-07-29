// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PlayerAnimInstance.generated.h"

class APlayerCharacter;
class UAbilitySystemComponent;
class APS_Player;
UCLASS()
class NEW_THEPHANTOMTWINS_API UPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UPlayerAnimInstance();

	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	virtual void NativePostEvaluateAnimation() override;

protected:
	TObjectPtr<APlayerCharacter> Owner;
	TObjectPtr<UAbilitySystemComponent> ASC;
	TObjectPtr<APS_Player> PS;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AnimProperty")
	float Speed;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AnimProperty")
	bool bIsGroggy;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AnimProperty")
	bool bIsRecovery;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AnimProperty")
	bool bIsInteractive;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AnimProperty")
	bool bIsActiveSkill;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AnimProperty")
	bool bIsHide;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AnimProperty")
	bool bIsCrouch;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AnimProperty")
	bool bIsInteract;

private:

};
