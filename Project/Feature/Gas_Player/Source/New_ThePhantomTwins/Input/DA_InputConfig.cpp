// Fill out your copyright notice in the Description page of Project Settings.


#include "DA_InputConfig.h"

const UInputAction* UDA_InputConfig::FindInputActionForTag(const FGameplayTag& InputTag) const
{
    for (const FTaggedInputAction& Pair : TaggedInputActions)
    {
        if (Pair.InputTag == InputTag)
            return Pair.InputAction;
    }
    return nullptr;
}
