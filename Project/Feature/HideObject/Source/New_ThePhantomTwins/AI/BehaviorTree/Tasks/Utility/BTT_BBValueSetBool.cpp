// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_BBValueSetBool.h"
#include "Behaviortree/Blackboardcomponent.h"

#include "Log/TPTLog.h"

UBTT_BBValueSetBool::UBTT_BBValueSetBool()
{
	NodeName = TEXT("BBValueSetBool");
}

EBTNodeResult::Type UBTT_BBValueSetBool::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();
	NULLCHECK_RETURN_LOG(BBComp, AILog, Warning, EBTNodeResult::Failed);

	BBComp->SetValueAsBool(BBKey.SelectedKeyName, Value);

	return EBTNodeResult::Succeeded;
}

FString UBTT_BBValueSetBool::GetStaticDescription() const
{
	return FString("Bool Value Set from BB.");
}
