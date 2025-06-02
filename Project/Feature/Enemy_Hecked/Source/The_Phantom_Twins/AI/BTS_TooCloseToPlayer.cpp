// Fill out your copyright notice in the Description page of Project Settings.


#include "BTS_TooCloseToPlayer.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Pawn.h"
#include "AIInterface.h"
#include "MyAICharacter.h"
#include "MyAIController.h"

UBTS_TooCloseToPlayer::UBTS_TooCloseToPlayer()
{
	Interval = 0.5f; // 0.5초마다 실행
	bNotifyBecomeRelevant = true;
}

void UBTS_TooCloseToPlayer::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
    AMyAIController* AIController = Cast<AMyAIController>(OwnerComp.GetAIOwner());
    if (!AIController)
    {
        return;
    }

    AMyAICharacter* AIPawn = Cast<AMyAICharacter>(AIController->GetPawn());
    if (!AIPawn)
    {
        return;
    }

    UWorld* World = AIPawn->GetWorld();
    if (World == nullptr)
    {
        return;
    }

    APawn* ClosestPlayerPawn = nullptr;
    float ClosestDistance = MaxDistance;

    for (FConstPlayerControllerIterator Iterator = World->GetPlayerControllerIterator(); Iterator; ++Iterator)
    {
        APlayerController* PlayerController = Iterator->Get();
        if (PlayerController == nullptr)
        {
            continue;
        }

        APawn* PlayerPawn = PlayerController->GetPawn();
        if (PlayerPawn == nullptr || PlayerPawn == AIPawn)
        {
            continue;
        }

        float Distance = FVector::Dist(AIPawn->GetActorLocation(), PlayerPawn->GetActorLocation());
        if (Distance < ClosestDistance)
        {
            ClosestPlayerPawn = PlayerPawn;
            ClosestDistance = Distance;
        }
    }

    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
    if (BlackboardComp == nullptr)
    {
        return;
    }

    if (ClosestPlayerPawn != nullptr)
    {
        // 상태변화 시키기.
        BlackboardComp->SetValueAsEnum("AIState", static_cast<uint8>(EMyAIState::Combat));
        BlackboardComp->SetValueAsObject("TargetPlayer", ClosestPlayerPawn);
    }

}
