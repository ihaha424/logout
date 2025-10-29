// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Utility/GameplayTagNavLinkComponent.h"
#include "NavModifierComponent.h"
#include "NavAreas/NavArea_Null.h"
#include "NavAreas/NavArea_Default.h"
#include "NavAreas/NavArea_Obstacle.h"
#include "NavigationSystem.h"
#include "Navigation/PathFollowingComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "AIController.h"
#include "GameplayEventDataExistByTag.h"
#include "Log/TPTLog.h"

UGameplayTagNavLinkComponent::UGameplayTagNavLinkComponent()
{
    AbilityTag = FGameplayTag();
    TargetActor = nullptr;

    NavModifier = CreateDefaultSubobject<UNavModifierComponent>(TEXT("NavModifier"));
    NavModifier->SetAreaClass(UNavArea_Obstacle::StaticClass()); // 기본값: 차단
}

void UGameplayTagNavLinkComponent::BeginPlay()
{
    Super::BeginPlay();

    TargetActor = GetOwner();
    SetLinkData(LinkStartPos, LinkEndPos, ENavLinkDirection::BothWays);
    SetMoveReachedLink(this, &UGameplayTagNavLinkComponent::HandleSmartLinkReached);
}
#include "Kismet/KismetSystemLibrary.h"

void UGameplayTagNavLinkComponent::HandleSmartLinkReached(UNavLinkCustomComponent* LinkComp, UObject* PathingAgent, const FVector& DestPoint)
{
    UKismetSystemLibrary::PrintString(this, TEXT("11111"));

    AActor* MovingActor = nullptr;
    UPathFollowingComponent* PathComp = Cast<UPathFollowingComponent>(PathingAgent);
    if (PathComp)
    {
        MovingActor = PathComp->GetOwner();
        AController* Controller = Cast<AController>(MovingActor);
        if (Controller)
        {
            MovingActor = Controller->GetPawn();
        }
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

    if (MovingActor->GetClass()->ImplementsInterface(UGameplayEventDataExistByTag::StaticClass()))
    {
        FGameplayEventData TempData;
        if (IGameplayEventDataExistByTag::Execute_GetGameplayEventData(MovingActor, AbilityTag, TempData, TargetActor))
        {
            EventData.EventMagnitude = TempData.EventMagnitude;
        }
    }
    UKismetSystemLibrary::PrintString(this, FString::Printf(TEXT("Actor: %s"), *MovingActor->GetName()));

    ASC->HandleGameplayEvent(AbilityTag, &EventData);
}

void UGameplayTagNavLinkComponent::NavProxyEnabled(bool Setbool)
{
    if (Setbool)
    {
        SetEnabled(false); // Proxy 연결 끊기
        NavModifier->SetAreaClass(UNavArea_Default::StaticClass()); // NavMesh가 뚫리도록 설정

        // NavMesh 즉시 리빌드(필요시)
        //UNavigationSystemV1::GetCurrent(GetWorld())->Build();
    }
    else
    {
        SetEnabled(true); // Proxy 연결
        NavModifier->SetAreaClass(UNavArea_Obstacle::StaticClass()); // NavMesh 차단(블로킹)
        //UNavigationSystemV1::GetCurrent(GetWorld())->Build();
    }
}
