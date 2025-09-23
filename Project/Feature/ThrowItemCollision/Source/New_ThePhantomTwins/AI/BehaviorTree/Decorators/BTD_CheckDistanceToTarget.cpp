// Fill out your copyright notice in the Description page of Project Settings.


#include "BTD_CheckDistanceToTarget.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "Log/TPTLog.h"

UBTD_CheckDistanceToTarget::UBTD_CheckDistanceToTarget()
{
	NodeName = TEXT("CheckDistanceToTarget");

	bNotifyBecomeRelevant = true;
	bNotifyCeaseRelevant = false;
}

FString UBTD_CheckDistanceToTarget::GetStaticDescription() const
{
	return FString("Target and distance determination.");
}

bool UBTD_CheckDistanceToTarget::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	NULLCHECK_RETURN_LOG(BB, AILog, Warning, false);

	AActor* Target = Cast<AActor>(BB->GetValueAsObject(TargetActor.SelectedKeyName));
	NULLCHECK_RETURN_LOG(Target, AILog, Warning, false);

	AAIController* AIController = OwnerComp.GetAIOwner();
	NULLCHECK_RETURN_LOG(AIController, AILog, Warning, false);

	APawn* AIPawn = AIController->GetPawn();
	NULLCHECK_RETURN_LOG(AIPawn, AILog, Warning, false);

	FVector MyLoc = AIPawn->GetActorLocation();
	FVector TargetLoc = Target->GetActorLocation();
	const float CurDistance = FVector::Dist(MyLoc, TargetLoc);
	if (CurDistance <= Distance)
	{
		FHitResult HitResult;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(AIPawn);

		bool bHit = GetWorld()->LineTraceSingleByChannel(
			HitResult,
			MyLoc,
			TargetLoc,
			ECC_Pawn,
			Params
		);
#if WITH_EDITOR
		//DrawDebugLine(
		//	GetWorld(),
		//	MyLoc,
		//	TargetLoc,
		//	FColor::Red,
		//	false,
		//	2.0f,
		//	0,
		//	2.0f
		//);
#endif
		if (bHit)
		{
			if (HitResult.GetActor() == Target)
				return true;
		}
	}

	return false;
}
