// Fill out your copyright notice in the Description page of Project Settings.


#include "AIBaseAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"

#include "Tags/TPTGameplayTags.h"

UAIBaseAttributeSet::UAIBaseAttributeSet() {}

void UAIBaseAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
    Super::PostGameplayEffectExecute(Data);
}
