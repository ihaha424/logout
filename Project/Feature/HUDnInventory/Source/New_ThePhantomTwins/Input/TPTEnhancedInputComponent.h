// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DA_InputConfig.h"
#include "EnhancedInputComponent.h"
#include "GameplayTagContainer.h"
#include "New_ThePhantomTwins/Tags/TPTGameplayTags.h"
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
			const EFTPTGameplayTags* TagEnum = FTPTGameplayTags::Get().TagMap.Find(InputTag);
			int32 InputID = static_cast<int32>(*TagEnum);
			BindAction(IA, TriggerEvent, Object, Func , InputID);
		}
	}

	template<class UserClass, typename FuncType>
	void BindActionByTag(const UDA_InputConfig* InputConfig, const FGameplayTag& InputTag, ETriggerEvent TriggerEvent, UserClass* Object, FuncType Func, int32 Number)
	{
		check(InputConfig);
		if (const UInputAction* IA = InputConfig->FindInputActionForTag(InputTag))
		{
			const EFTPTGameplayTags* TagEnum = FTPTGameplayTags::Get().TagMap.Find(InputTag);
			int32 InputID = static_cast<int32>(*TagEnum);
			BindAction(IA, TriggerEvent, Object, Func, InputID, Number);
		}
	}
};