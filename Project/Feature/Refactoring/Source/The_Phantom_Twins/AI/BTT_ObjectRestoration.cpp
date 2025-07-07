#include "BTT_ObjectRestoration.h"
#include "MyAIController.h"
#include "MyAICharacter.h"
#include <SzObjects/HackableObject.h>

#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"

UBTT_ObjectRestoration::UBTT_ObjectRestoration()
{
	NodeName = TEXT("Turn Off Object");
	bNotifyTick = true; // TickTask 활성화
}

EBTNodeResult::Type UBTT_ObjectRestoration::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);
	return EBTNodeResult::InProgress; // TickTask에서 처리
}

void UBTT_ObjectRestoration::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	AMyAIController* AIController = Cast<AMyAIController>(OwnerComp.GetAIOwner());
	if (!AIController)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	AMyAICharacter* AIPawn = Cast<AMyAICharacter>(AIController->GetPawn());
	if (!AIPawn)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}
	UBlackboardComponent* BlackboardComp = AIController->GetBlackboardComponent();
	if (!BlackboardComp)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}
	AHackableObject* Target = Cast<AHackableObject>(BlackboardComp->GetValueAsObject(TEXT("TargetObject")));
	if (!Target)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}
	float Distance = FVector::Dist(AIPawn->GetActorLocation(), Target->GetActorLocation());
	FNavLocation NavMeshLocation;
	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
	if (NavSys && NavSys->ProjectPointToNavigation(Target->GetActorLocation(), NavMeshLocation, FVector(300, 300, 700)))
	{
		Target->GetActorLocation() = NavMeshLocation; // NavMesh 위 좌표로 보정
	}
	if (Distance < 300)
	{
		//Target->ClearHacking();
		IHacking::Execute_ClearHacking(Target, Cast<APawn>(Target));
		BlackboardComp->ClearValue(TEXT("TargetObject"));
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
	FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
}
