// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayTagNavLinkProxy.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"

#include "Log/TPTLog.h"

AGameplayTagNavLinkProxy::AGameplayTagNavLinkProxy()
{
    bSmartLinkIsRelevant = true;

    AbilityTag = FGameplayTag();
    TargetActor = nullptr;
}

void AGameplayTagNavLinkProxy::BeginPlay()
{
    Super::BeginPlay();
    OnSmartLinkReached.AddDynamic(this, &AGameplayTagNavLinkProxy::HandleSmartLinkReached);
}

void AGameplayTagNavLinkProxy::HandleSmartLinkReached(AActor* MovingActor, const FVector& DestinationPoint)
{
    if (!IsValid(TargetActor))
    {
        OnSmartLinkReached.RemoveDynamic(this, &AGameplayTagNavLinkProxy::HandleSmartLinkReached);
        return;
    }
    CONDITIONCHECK_RETURN_LOG(!AbilityTag.IsValid(), AILog, Warning, );
    UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(MovingActor);
    NULLCHECK_RETURN_LOG(ASC, AILog, Warning, );

    FGameplayEventData EventData;
    EventData.Instigator = MovingActor;
    EventData.Target = TargetActor;

    ASC->HandleGameplayEvent(AbilityTag, &EventData);
}
