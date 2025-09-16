// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BehaviorTree/Tasks/Ability/BTT_ApplyEffectByTarget.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "GameplayTagContainer.h"
#include "Log/TPTLog.h"


UBTT_ApplyEffectByTarget::UBTT_ApplyEffectByTarget()
{
    NodeName = TEXT("Apply GamePlayEffect");
    TargetActorKey = FBlackboardKeySelector();
    bNotifyTick = false;
}

EBTNodeResult::Type UBTT_ApplyEffectByTarget::Execute_Task(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    if (!TargetActorKey.SelectedKeyName.IsValid())
        return EBTNodeResult::Failed;

    AActor* TargetActor = nullptr;
    UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
    TargetActor = Cast<AActor>(BB->GetValueAsObject(TargetActorKey.SelectedKeyName));

    UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetActor);
    if (!ASC) 
        return EBTNodeResult::Failed;

    FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
    EffectContext.AddSourceObject(this);

    FGameplayEffectSpecHandle EffectSpecHandle = ASC->MakeOutgoingSpec(GameplayEffectClass, 1, EffectContext);
    if (!EffectSpecHandle.IsValid())
        return EBTNodeResult::Failed;

    FActiveGameplayEffectHandle ActivcEffecttHandle = ASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
    if (bHasDuration)
    {
        const FActiveGameplayEffectHandle HandleCopy = ActivcEffecttHandle;          
        const TWeakObjectPtr<UAbilitySystemComponent> WeakASC = ASC;                 

        FTimerHandle TimerHandle;
        GetWorld()->GetTimerManager().SetTimer(
            TimerHandle,
            FTimerDelegate::CreateWeakLambda(
                this,                              
                [WeakASC, HandleCopy]()            
                {
                    if (UAbilitySystemComponent* ASCResolved = WeakASC.Get())
                    {
                        if (HandleCopy.IsValid())
                        {
                            ASCResolved->RemoveActiveGameplayEffect(HandleCopy);
                        }
                    }
                }
            ),
            Duration,
            false
        );
    }

    return EBTNodeResult::Succeeded;
}

FString UBTT_ApplyEffectByTarget::GetStaticDescription() const
{
    return FString::Printf(TEXT("Effect : %s | Target: Set BlackBoard Key"), IsValid(GameplayEffectClass) ? *GameplayEffectClass.Get()->GetName() : *FString("None"));
}
