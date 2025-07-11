// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTT_PlayParticle.generated.h"


class UNiagaraSystem;

UCLASS()
class THE_PHANTOM_TWINS_API UBTT_PlayParticle : public UBTTaskNode
{
	GENERATED_BODY()
public:
    UBTT_PlayParticle();

protected:
    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
    

    void PlayParticle(ACharacter* Character);
    UFUNCTION(NetMulticast, Reliable)
    void S2A_PlayParticle(ACharacter* Character);
    void S2A_PlayParticle_Implementation(ACharacter* Character);

public:
    UPROPERTY(EditAnywhere, Category = "Effect")
    bool bIsNetMulticast = false;

    UPROPERTY(EditAnywhere, Category = "Effect")
    UNiagaraSystem* ParticleSystem;

    UPROPERTY(EditAnywhere, Category = "Effect")
    FName SocketName = NAME_None;

    UPROPERTY(EditAnywhere, Category = "Effect")
    FVector LocationOffset = FVector::ZeroVector;
};
