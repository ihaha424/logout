// Fill out your copyright notice in the Description page of Project Settings.


#include "BTD_CheckDistanceToTarget.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "SzInterface/Destroyable.h"
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
	FVector MyLocDistance = MyLoc;
	MyLocDistance.Z = 0;
	FVector TargetLocDistance = TargetLoc;
	TargetLocDistance.Z = 0;
	const float CurDistance = FVector::Dist(MyLocDistance, TargetLocDistance);

	if (CurDistance <= Distance + DistanceThreshold)
	{
		FHitResult HitResult;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(AIPawn);

		FCollisionObjectQueryParams ObjParams;
		ObjParams.AddObjectTypesToQuery(ECC_GameTraceChannel1);
		ObjParams.AddObjectTypesToQuery(ECC_Pawn);
		ObjParams.AddObjectTypesToQuery(ECC_Visibility);

		const bool bHit = GetWorld()->LineTraceSingleByObjectType(
			HitResult, MyLoc, TargetLoc, ObjParams, Params
		);

//#if WITH_EDITOR
//		DrawDebugLine(
//			GetWorld(),
//			MyLoc,
//			TargetLoc,
//			FColor::Red,
//			false,
//			2.0f,
//			0,
//			5.0f
//		);
//#endif
		if (bHit)
		{
			AActor* HitActor = HitResult.GetActor();
			if (HitActor)
			{

				if (HitActor && HitActor->GetClass()->ImplementsInterface(UDestroyable::StaticClass()))
				{
					BB->SetValueAsObject(ObjectActorKey.SelectedKeyName, HitActor);
					BB->SetValueAsBool(bSmashObjectKey.SelectedKeyName, true);
					return true;
				}

				if (HitActor == Target && HitResult.Distance <= Distance)
				{

					return true;
				}
			}
		}
	}
	return false;
}
