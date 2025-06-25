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
	//UE_LOG(LogTemp, Error, TEXT("rkddlsrbrpdl ObjectClear Start"));
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
	AActor* Target = Cast<AActor>(BlackboardComp->GetValueAsObject(TEXT("TargetObject")));
	if (nullptr == Target || !Target->GetClass()->ImplementsInterface(UHacking::StaticClass()))
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}
	FVector MyLocation = AIPawn->GetActorLocation();
	FVector TargetLocation = Target->GetActorLocation();

	MyLocation.Z = 0;
	TargetLocation.Z = 0;

	float Distance2D = FVector::Dist2D(MyLocation, TargetLocation); // XY 평면 거리

	//UE_LOG(LogTemp, Warning, TEXT("%f"), Distance2D);
	if (Distance2D < 500)
	{
		IHacking::Execute_ClearHacking(Target);
		BlackboardComp->ClearValue(TEXT("TargetObject"));
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		//UE_LOG(LogTemp, Error, TEXT("rkddlsrbrpdl ObjectClear"));

		return;
	}
	//UE_LOG(LogTemp, Error, TEXT("rkddlsrbrpdl ObjectClear Fail"));
	FinishLatentTask(OwnerComp, EBTNodeResult::Failed);

}
