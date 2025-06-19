// Fill out your copyright notice in the Description page of Project Settings.

#include "BTT_AISenseRestoration.h"

#include "AIInterface.h"
#include "MyAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISense_Hearing.h"

UBTT_AISenseRestoration::UBTT_AISenseRestoration()
{
	NodeName = TEXT("AISense Restoration");
}

EBTNodeResult::Type UBTT_AISenseRestoration::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    Super::ExecuteTask(OwnerComp, NodeMemory);
    AMyAIController* AIController = Cast<AMyAIController>(OwnerComp.GetAIOwner());
    if (!AIController)
    {
        return EBTNodeResult::Failed;
    }
    UBlackboardComponent* BlackboardComp = AIController->GetBlackboardComponent();
    if (!BlackboardComp)
    {
        return EBTNodeResult::Failed;
    }
    // 감각 복원
    if (UAIPerceptionComponent* Perception = AIController->FindComponentByClass<UAIPerceptionComponent>())
    {
        Perception->SetSenseEnabled(UAISense_Sight::StaticClass(), true);
        Perception->SetSenseEnabled(UAISense_Hearing::StaticClass(), true);
        BlackboardComp->SetValueAsEnum("AIState", static_cast<uint8>(EMyAIState::Default));

    }
	return EBTNodeResult::Succeeded;
}
