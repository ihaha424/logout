// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/ScanEffectActor.h"

AScanEffectActor::AScanEffectActor()
{
    PrimaryActorTick.bCanEverTick = true;
    bAutoDestroyOnRemove = true;
    bAutoAttachToOwner = true;
}

bool AScanEffectActor::OnActive_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters)
{
	SetActorRelativeLocation(GetActorLocation());

	return true;
}

bool AScanEffectActor::WhileActive_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters)
{
	SetActorTickEnabled(true);

	return true;
}

bool AScanEffectActor::OnRemove_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters)
{
	SetActorTickEnabled(true);

	return true;
}
