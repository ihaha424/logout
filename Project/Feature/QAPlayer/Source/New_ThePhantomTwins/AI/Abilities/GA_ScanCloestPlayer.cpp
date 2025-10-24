// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Abilities/GA_ScanCloestPlayer.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "AI/Controller/AIBaseController.h"
#include "AI/Character/AIBaseCharacter.h"
#include "Tags/TPTGameplayTags.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Log/TPTLog.h"


UGA_ScanCloestPlayer::UGA_ScanCloestPlayer()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;
    ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;

    AbilityTags.AddTag(FTPTGameplayTags::Get().TPTGameplay_Character_Action_ScanCloestPlayer);

    FAbilityTriggerData TriggerData;
    TriggerData.TriggerTag = FTPTGameplayTags::Get().TPTGameplay_Character_Action_ScanCloestPlayer;
    TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
    AbilityTriggers.Add(TriggerData);
}

void UGA_ScanCloestPlayer::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    AActor* thisActor = ActorInfo->AvatarActor.Get();
    AAIBaseCharacter* AI = Cast<AAIBaseCharacter>(thisActor);
    AActor* ClosestActor = nullptr;
    float Distance = std::numeric_limits<float>::max();
    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        APlayerController* PC = It->Get();
        if (nullptr == PC) continue;
        APawn* Pawn = PC->GetPawn();
        if (nullptr == Pawn) continue;
        UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Pawn);
        if (!IsValid(ASC))
            continue;
        if (ASC->HasMatchingGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_State_Downed))
            continue;

        float DistSq = thisActor->GetDistanceTo(Pawn);

        if (DistSq < Distance)
        {
            Distance = DistSq;
            ClosestActor = Pawn;
        }
    }

    if (nullptr != ClosestActor)
    {
        AAIBaseController* AIController = Cast<AAIBaseController>(AI->GetController());
        AIController->AddPerceptionSightList(ClosestActor);
    }

    EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
