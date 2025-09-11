// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_SetState.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AI/Utility/AIHelperLibrary.h"

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

    EAIBaseState CurrentState = static_cast<EAIBaseState>(BBComp->GetValueAsEnum(AIStateKey.SelectedKeyName));
    if (StateValue == CurrentState)
        return EBTNodeResult::Failed;

    UAIHelperLibrary::SetAIStateAndTag(&OwnerComp, BBComp, CurrentState, StateValue, AIStateKey.SelectedKeyName);

    return EBTNodeResult::Succeeded;
}

FString UBTT_SetState::GetStaticDescription() const
{
    const UEnum* EnumPtr = StaticEnum<EAIBaseState>();
    if (!EnumPtr) return TEXT("EAIBaseState: Invalid");

    return FString::Printf(TEXT("SetState: %s"), *EnumPtr->GetDisplayNameTextByValue((int64)StateValue).ToString());
}
