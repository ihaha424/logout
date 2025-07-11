// Fill out your copyright notice in the Description page of Project Settings.


#include "BTS_TooCloseToPlayer.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Pawn.h"
#include "AIInterface.h"
#include "MyAICharacter.h"
#include "MyAIController.h"
#include "The_Phantom_Twins/Player/PlayerDefaultController.h"
#include "The_Phantom_Twins/Player/PlayerBase.h"

UBTS_TooCloseToPlayer::UBTS_TooCloseToPlayer()
{
	Interval = 0.5f; // 0.5초마다 실행
	bNotifyBecomeRelevant = true;
}

void UBTS_TooCloseToPlayer::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

    AMyAIController* AIController = Cast<AMyAIController>(OwnerComp.GetAIOwner());
    if (!AIController) return;

    AMyAICharacter* AIPawn = Cast<AMyAICharacter>(AIController->GetPawn());
    if (!AIPawn) return;

    UWorld* World = AIPawn->GetWorld();
    if (!World) return;

    APawn* ClosestPlayerPawn = nullptr;
    float ClosestDistance = MaxDistance;

    for (FConstPlayerControllerIterator Iterator = World->GetPlayerControllerIterator(); Iterator; ++Iterator)
    {
        APlayerDefaultController* PC = Cast<APlayerDefaultController>(Iterator->Get());
        if (!PC) continue;

        APlayerBase* Player = Cast<APlayerBase>(PC->GetPawn());
        if (!Player) continue;

        FVector MyLocation = AIPawn->GetActorLocation();
        FVector TargetLocation = Player->GetActorLocation();
        MyLocation.Z = 0;
        TargetLocation.Z = 0;
        float Distance = FVector::Dist(MyLocation, TargetLocation);
        if (Distance < ClosestDistance)
        {
            ClosestDistance = Distance;
            ClosestPlayerPawn = Player;
        }
    }

    UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
    if (!BB) return;

    if (ClosestPlayerPawn && ClosestDistance <= MaxDistance)
    {
    	BB->SetValueAsEnum("AIState", static_cast<uint8>(EMyAIState::Combat));
        BB->SetValueAsObject("ChasingPlayer", ClosestPlayerPawn);
        AIController->SetSightForgetTime(3.f);
    }
}