// Fill out your copyright notice in the Description page of Project Settings.


#include "DA_InputConfig.h"

const UInputAction* UDA_InputConfig::FindInputActionForTag(const FGameplayTag& InputTag) const
{
    for (const FTaggedInputAction& Action : TaggedInputActions)
    {
        if (Action.InputTag == InputTag)
            return Action.InputAction;
    }
    return nullptr;
}
