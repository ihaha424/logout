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
public:
    ATTRIBUTE_ACCESSORS(UAIBaseAttributeSet, AttackPower);
    ATTRIBUTE_ACCESSORS(UAIBaseAttributeSet, MentalityAttackPower);
    ATTRIBUTE_ACCESSORS(UAIBaseAttributeSet, AttackCoolTime);

protected:
    UPROPERTY(ReplicatedUsing = OnRep_AttackPower, BlueprintReadWrite, Category = "Attributes|Attack")
    FGameplayAttributeData AttackPower;
    UPROPERTY(ReplicatedUsing = OnRep_MentalityAttackPower, BlueprintReadWrite, Category = "Attributes|Attack")
    FGameplayAttributeData MentalityAttackPower;
    UPROPERTY(ReplicatedUsing = OnRep_AttackCoolTime, BlueprintReadWrite, Category = "Attributes|Attack")
    FGameplayAttributeData AttackCoolTime;
    //~~~   End Attack Power


    //~~~   Begin Perception Stat
public:
    ATTRIBUTE_ACCESSORS(UAIBaseAttributeSet, SightRadius);
    ATTRIBUTE_ACCESSORS(UAIBaseAttributeSet, LoseSightRadius);
    ATTRIBUTE_ACCESSORS(UAIBaseAttributeSet, SightAngle);
    ATTRIBUTE_ACCESSORS(UAIBaseAttributeSet, SightAge);
    ATTRIBUTE_ACCESSORS(UAIBaseAttributeSet, HearingRange);
    ATTRIBUTE_ACCESSORS(UAIBaseAttributeSet, HearingAge);
    ATTRIBUTE_ACCESSORS(UAIBaseAttributeSet, ImmediateDetectionRange);

protected:
    UPROPERTY(BlueprintReadWrite, Category = "Attributes|Perception")
    FGameplayAttributeData SightRadius;
    UPROPERTY(BlueprintReadWrite, Category = "Attributes|Perception")
    FGameplayAttributeData LoseSightRadius;
    UPROPERTY(BlueprintReadWrite, Category = "Attributes|Perception")
    FGameplayAttributeData SightAngle;
    UPROPERTY(BlueprintReadWrite, Category = "Attributes|Perception")
    FGameplayAttributeData SightAge;
    UPROPERTY(BlueprintReadWrite, Category = "Attributes|Perception")
    FGameplayAttributeData HearingRange;
    UPROPERTY(BlueprintReadWrite, Category = "Attributes|Perception")
    FGameplayAttributeData HearingAge;
    UPROPERTY(BlueprintReadWrite, Category = "Attributes|Perception")
    FGameplayAttributeData ImmediateDetectionRange;
    //~~~   End Perception Stat


    //~~~   Begin Move Speed
public:
    ATTRIBUTE_ACCESSORS(UAIBaseAttributeSet, MoveSpeed);
    ATTRIBUTE_ACCESSORS(UAIBaseAttributeSet, ChaseMoveSpeed);

protected:
    UPROPERTY(ReplicatedUsing = OnRep_MoveSpeed, BlueprintReadWrite, Category = "Attributes|Move Speed")
    FGameplayAttributeData MoveSpeed;
    UPROPERTY(ReplicatedUsing = OnRep_ChaseMoveSpeed, BlueprintReadWrite, Category = "Attributes|Move Speed")
    FGameplayAttributeData ChaseMoveSpeed;
    //~~~   End Move Speed
    //~ End Property

public:
    UFUNCTION()
    void OnRep_AttackPower(const FGameplayAttributeData& OldValue);
    UFUNCTION()
    void OnRep_MentalityAttackPower(const FGameplayAttributeData& OldValue);
    UFUNCTION()
    void OnRep_AttackCoolTime(const FGameplayAttributeData& OldValue);
    UFUNCTION()
    void OnRep_MoveSpeed(const FGameplayAttributeData& OldValue);
    UFUNCTION()
    void OnRep_ChaseMoveSpeed(const FGameplayAttributeData& OldValue);

protected:
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
