// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_PlayParticle.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "AIController.h"
#include "GameFramework/Character.h"

UBTT_PlayParticle::UBTT_PlayParticle()
{
	NodeName = TEXT("Play Niagara Particle");
}

EBTNodeResult::Type UBTT_PlayParticle::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AICon = OwnerComp.GetAIOwner();
    if (!AICon) return EBTNodeResult::Failed;

    ACharacter* Character = Cast<ACharacter>(AICon->GetPawn());
    if (!Character || !ParticleSystem) return EBTNodeResult::Failed;

    if (!bIsNetMulticast)
        PlayParticle(Character);
    else
        S2A_PlayParticle(Character);


    return EBTNodeResult::Succeeded;
}

void UBTT_PlayParticle::PlayParticle(ACharacter* Character)
{
    UNiagaraFunctionLibrary::SpawnSystemAttached(
        ParticleSystem,
        Character->GetMesh(),
        SocketName,
        LocationOffset,
        FRotator::ZeroRotator,
        EAttachLocation::KeepRelativeOffset,
        true
    );
}
void UBTT_PlayParticle::S2A_PlayParticle_Implementation(ACharacter* Character)
{
    PlayParticle(Character);
}
