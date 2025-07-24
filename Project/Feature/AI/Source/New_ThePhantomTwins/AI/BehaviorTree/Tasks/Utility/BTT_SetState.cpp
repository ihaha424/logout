// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_SetState.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTT_SetState::UBTT_SetState()
{
	NodeName = "SetState";

    AIStateKey = FBlackboardKeySelector();
	StateValue = EAIBaseState::Default;
}

EBTNodeResult::Type UBTT_SetState::Execute_Task(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();
    if (!ensureMsgf(BBComp != nullptr, TEXT("[UBTT_SetState] BlackboardComponent is missing!")))
        return EBTNodeResult::Failed;

    if (static_cast<uint8>(StateValue) == BBComp->GetValueAsEnum(AIStateKey.SelectedKeyName))
        return EBTNodeResult::Failed;

    BBComp->SetValueAsEnum(AIStateKey.SelectedKeyName, static_cast<uint8>(StateValue));

    return EBTNodeResult::Succeeded;
}

FString UBTT_SetState::GetStaticDescription() const
{
    const UEnum* EnumPtr = StaticEnum<EAIBaseState>();
    if (!EnumPtr) return TEXT("EAIBaseState: Invalid");

    return FString::Printf(TEXT("SetState: %s"), *EnumPtr->GetDisplayNameTextByValue((int64)StateValue).ToString());
}
