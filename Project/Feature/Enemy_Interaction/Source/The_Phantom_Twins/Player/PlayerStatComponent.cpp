// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerStatComponent.h"

// Sets default values for this component's properties
UPlayerStatComponent::UPlayerStatComponent()
{
	MaxHp = 1;
	SetHp(MaxHp);
}
	


// Called when the game starts
void UPlayerStatComponent::BeginPlay()
{
	Super::BeginPlay();

	SetHp(MaxHp);
}


float UPlayerStatComponent::ApplyDamage(float Damage)
{
	const float PrevHp = CurretHp;
	const float ActualDamage = FMath::Clamp(Damage, 0.f, MaxHp);

	SetHp(PrevHp - ActualDamage);

	if (CurretHp <= KINDA_SMALL_NUMBER)
	{
		// Á×¾úÀ» ¶§
		OnHpZero.Broadcast();
	}

	return ActualDamage;
}

void UPlayerStatComponent::SetHp(float NewHp)
{
	CurretHp = FMath::Clamp<float>(NewHp, 0.f, MaxHp);

	OnHpChanged.Broadcast(CurretHp);
}
