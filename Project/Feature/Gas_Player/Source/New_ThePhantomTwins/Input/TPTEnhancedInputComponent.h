// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DA_InputConfig.h"
#include "EnhancedInputComponent.h"
#include "GameplayTagContainer.h"
#include "TPTEnhancedInputComponent.generated.h"

/**
 * 
 */
UCLASS()
class NEW_THEPHANTOMTWINS_API UTPTEnhancedInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()
public:

	template<class UserClass, typename FuncType>
	void BindActionByTag(const UDA_InputConfig* InputConfig, const FGameplayTag& InputTag, ETriggerEvent TriggerEvent, UserClass* Object, FuncType Func)
	{
		check(InputConfig);
		if (const UInputAction* IA = InputConfig->FindInputActionForTag(InputTag))
		{
			BindAction(IA, TriggerEvent, Object, Func);
		}
	}
};
