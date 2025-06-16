// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_AttackPlayer.h"

#include "AIInterface.h"
#include "MyAICharacter.h"
#include "MyAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "The_Phantom_Twins/Player/PlayerBase.h"

UBTT_AttackPlayer::UBTT_AttackPlayer()
{
	NodeName = TEXT("Attack Player");
}

EBTNodeResult::Type UBTT_AttackPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);
	AMyAIController* AIController = Cast<AMyAIController>(OwnerComp.GetAIOwner());
	if (!AIController)
	{
		return EBTNodeResult::Failed;
	}

	AMyAICharacter* AIPawn = Cast<AMyAICharacter>(AIController->GetPawn());
	if (!AIPawn)
	{
		return EBTNodeResult::Failed;
	}
	UBlackboardComponent* BlackboardComp = AIController->GetBlackboardComponent();
	if (!BlackboardComp)
	{
		return EBTNodeResult::Failed;
	}
	APlayerBase* Target = Cast<APlayerBase>(BlackboardComp->GetValueAsObject(TEXT("TargetPlayer")));
	if (!Target)
	{
		return EBTNodeResult::Failed;
	}

	if (Target->IsGroggy())
	{
		BlackboardComp->SetValueAsEnum("AIState", static_cast<uint8>(EMyAIState::Default));
		//UE_LOG(LogTemp, Warning, TEXT("Target->IsGroggy()"));
		AIController->ResetStimulus();
		return EBTNodeResult::Succeeded;
	}

	UGameplayStatics::ApplyDamage(
		Target,          // 피해자
		DamageAmount,         // 데미지 양
		AIController,         // Instigator Controller
		AIPawn,               // 데미지를 유발한 액터
		UDamageType::StaticClass() // 기본 데미지 타입
	);
	//UE_LOG(LogTemp, Warning, TEXT("Attack Success()"));
	return EBTNodeResult::InProgress;
}
