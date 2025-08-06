// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Utility/GameplayTagNavLinkComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "AIController.h"
#include "Log/TPTLog.h"

UGameplayTagNavLinkComponent::UGameplayTagNavLinkComponent()
{
    AbilityTag = FGameplayTag();
    TargetActor = nullptr;
}

void UGameplayTagNavLinkComponent::BeginPlay()
{
    Super::BeginPlay();

    TargetActor = GetOwner();
    SetLinkData(FVector{ -100.0f, 0.0f, 0.0f }, FVector{ 100.0f, 0.0f, 0.0f }, ENavLinkDirection::BothWays);
    SetMoveReachedLink(this, &UGameplayTagNavLinkComponent::HandleSmartLinkReached);
}

void UGameplayTagNavLinkComponent::HandleSmartLinkReached(UNavLinkCustomComponent* LinkComp, UObject* PathingAgent, const FVector& DestPoint)
{
    AActor* MovingActor = nullptr;
    UPathFollowingComponent* PathComp = Cast<UPathFollowingComponent>(PathingAgent);
    if (PathComp)
    {
        MovingActor = PathComp->GetOwner();
        AController* Controller = Cast<AController>(MovingActor);
        if (Controller)
            MovingActor = Controller->GetPawn();
    }
    else
    {
        MovingActor = Cast<AActor>(PathingAgent);
    }

    if (!IsValid(MovingActor) || !IsValid(TargetActor))
        return;

    CONDITIONCHECK_RETURN_LOG(!AbilityTag.IsValid(), AILog, Warning, );

    UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(MovingActor);
    NULLCHECK_RETURN_LOG(ASC, AILog, Warning, );

    FGameplayEventData EventData;
    EventData.Instigator = MovingActor;
    EventData.Target = TargetActor;

    ASC->HandleGameplayEvent(AbilityTag, &EventData);
}
