// Fill out your copyright notice in the Description page of Project Settings.


#include "AIBaseCharacter.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "../Attributes/AIBaseAttributeSet.h"
#include "../Tags/TPTGameplayTags.h"


AAIBaseCharacter::AAIBaseCharacter()
{
    PrimaryActorTick.bCanEverTick = false;

    AbilitySystem = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystem"));
    AttributeSet = CreateDefaultSubobject<UAIBaseAttributeSet>(TEXT("AttributeSet"));

    AutoPossessPlayer = EAutoReceiveInput::Disabled;
    AIControllerClass = nullptr;

    bUseControllerRotationYaw = false;
    GetCharacterMovement()->bOrientRotationToMovement = true;
}

void AAIBaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
    if (AbilitySystem)
    {
        AbilitySystem->InitAbilityActorInfo(this, this);


        // TODO: 상태 변화시 함수 바인딩 -> 바인된 함수는 상태에 따른 효과 제공
        //AbilitySystem->RegisterGameplayTagEvent(CombatTag, EGameplayTagEventType::NewOrRemoved)
        //        .AddUObject(this, &UAIStateEffectComponent::OnCombatStateChanged);
        //AbilitySystem->RegisterGameplayTagEvent(StunnedTag, EGameplayTagEventType::NewOrRemoved)
        //        .AddUObject(this, &UAIStateEffectComponent::OnStunnedStateChanged);
        
    }
}

UAbilitySystemComponent* AAIBaseCharacter::GetAbilitySystemComponent() const
{
    return AbilitySystem;
}

UAIBaseAttributeSet* AAIBaseCharacter::GetAIAttributeSet() const
{
    return AttributeSet;
}

void AAIBaseCharacter::ApplyStun()
{
    AIStateTags.RemoveTag(FTPTGameplayTags::Get().TPTGamePlayTag_AIState_Combat);
    AIStateTags.AddTag(FTPTGameplayTags::Get().TPTGamePlayTag_AIState_Stun);

    // TODO: 움직임 멈추기, 몽타주 중단, 이펙트 재생 등
}

void AAIBaseCharacter::ResetToDefaultState()
{
    AIStateTags.Reset();
    AIStateTags.AddTag(FTPTGameplayTags::Get().TPTGamePlayTag_AIState_Default);

    // TODO: 원래 위치로 이동하거나 순찰 재시작
}

FString AAIBaseCharacter::GetCurrentAIStateAsString() const
{
    return AIStateTags.ToStringSimple();
}

void AAIBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AAIBaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

