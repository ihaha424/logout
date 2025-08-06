// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "AIBaseAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
    GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UCLASS()
class NEW_THEPHANTOMTWINS_API UAIBaseAttributeSet : public UAttributeSet
{
    GENERATED_BODY()

public:
    UAIBaseAttributeSet();

    //~ Begin UAttributeSet interface
    virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
    //~ End UAttributeSet interface


    //~ Begin Property
    //~~~   Begin Attack Power
    UPROPERTY(BlueprintReadWrite, Category = "Attributes|Attack")
    FGameplayAttributeData AttackPower;
    ATTRIBUTE_ACCESSORS(UAIBaseAttributeSet, AttackPower)

    UPROPERTY(BlueprintReadWrite, Category = "Attributes|Attack")
    FGameplayAttributeData MentalityAttackPower;
    ATTRIBUTE_ACCESSORS(UAIBaseAttributeSet, MentalityAttackPower)

    UPROPERTY(BlueprintReadWrite, Category = "Attributes|Attack")
    FGameplayAttributeData AttackCoolTime;
    ATTRIBUTE_ACCESSORS(UAIBaseAttributeSet, AttackCoolTime)
    //~~~   End Attack Power


    //~~~   Begin Perception Stat
    UPROPERTY(BlueprintReadWrite, Category = "Attributes|Perception")
    FGameplayAttributeData SightRadius;
    ATTRIBUTE_ACCESSORS(UAIBaseAttributeSet, SightRadius)
        
    UPROPERTY(BlueprintReadWrite, Category = "Attributes|Perception")
    FGameplayAttributeData LoseSightRadius;
    ATTRIBUTE_ACCESSORS(UAIBaseAttributeSet, LoseSightRadius)

    UPROPERTY(BlueprintReadWrite, Category = "Attributes|Perception")
    FGameplayAttributeData SightAngle;
    ATTRIBUTE_ACCESSORS(UAIBaseAttributeSet, SightAngle)

    UPROPERTY(BlueprintReadWrite, Category = "Attributes|Perception")
    FGameplayAttributeData SightAge;
    ATTRIBUTE_ACCESSORS(UAIBaseAttributeSet, SightAge)

    UPROPERTY(BlueprintReadWrite, Category = "Attributes|Perception")
    FGameplayAttributeData HearingRange;
    ATTRIBUTE_ACCESSORS(UAIBaseAttributeSet, HearingRange)

    UPROPERTY(BlueprintReadWrite, Category = "Attributes|Perception")
    FGameplayAttributeData HearingAge;
    ATTRIBUTE_ACCESSORS(UAIBaseAttributeSet, HearingAge)

    UPROPERTY(BlueprintReadWrite, Category = "Attributes|Perception")
    FGameplayAttributeData ImmediateDetectionRange;
    ATTRIBUTE_ACCESSORS(UAIBaseAttributeSet, ImmediateDetectionRange)
    //~~~   End Perception Stat


    //~~~   Begin Move Speed
    UPROPERTY(BlueprintReadWrite, Category = "Attributes|Move Speed")
    FGameplayAttributeData MoveSpeed;
    ATTRIBUTE_ACCESSORS(UAIBaseAttributeSet, MoveSpeed)

    UPROPERTY(BlueprintReadWrite, Category = "Attributes|Move Speed")
    FGameplayAttributeData ChaseMoveSpeed;
    ATTRIBUTE_ACCESSORS(UAIBaseAttributeSet, ChaseMoveSpeed)
    //~~~   End Move Speed
    //~ End Property

};
