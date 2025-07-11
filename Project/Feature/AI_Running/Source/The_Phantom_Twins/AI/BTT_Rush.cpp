// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_Rush.h"
#include "MyAICharacter.h"
#include "MyAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UBTT_Rush::UBTT_Rush()
{
    NodeName = TEXT("Rush");
    bNotifyTick = true;
    bNotifyTaskFinished = true;
}

EBTNodeResult::Type UBTT_Rush::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();;
    if (!BlackboardComp)
    {
        return EBTNodeResult::Failed;
    }
    ACharacter* Target = Cast<ACharacter>(BlackboardComp->GetValueAsObject(TEXT("TargetPlayer")));
    if (!Target)
    {
        return EBTNodeResult::Failed;
    }

    APawn* Pawn = OwnerComp.GetAIOwner()->GetPawn();
    if (Pawn)
    {
        Pawn->OnActorHit.AddDynamic(this, &UBTT_Rush::OnHit);

        RushDirection = Target->GetActorLocation() - Pawn->GetActorLocation();
        RushDirection.Normalize();

        CachedOwnerComp = &OwnerComp;
        CurrentSpeedPtr = (float*)NodeMemory;
        *CurrentSpeedPtr = InitialSpeed;

        bHitDetected = false;


        return EBTNodeResult::InProgress;
    }
    else
    {
        return EBTNodeResult::Failed;
    }
}

void UBTT_Rush::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    if (bHitDetected)
    {
        UE_LOG(LogTemp, Warning, TEXT("Rush: TickTask"));
        FinishLatentTask(*CachedOwnerComp, EBTNodeResult::Succeeded);
        return;
    }

    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController) return;

    APawn* Pawn = AIController->GetPawn();
    if (!Pawn) return;
    ACharacter* Character = Cast<ACharacter>(Pawn);
    if (!Character) return;

    float& CurrentSpeed = *(float*)NodeMemory;
    CurrentSpeed += Acceleration * DeltaSeconds;

    Character->GetCharacterMovement()->MaxWalkSpeed = MaxSpeed;
    Character->AddMovementInput(RushDirection, CurrentSpeed * DeltaSeconds);
}

void UBTT_Rush::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
    UE_LOG(LogTemp, Warning, TEXT("Rush: OnTaskFinished"));
    if (APawn* Pawn = OwnerComp.GetAIOwner()->GetPawn())
    {
        Pawn->OnActorHit.RemoveDynamic(this, &UBTT_Rush::OnHit);
    }
    UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
    if (!BB) return;
    BB->SetValueAsEnum("AIState", static_cast<uint8>(EMyAIState::Hacked));
}

uint16 UBTT_Rush::GetInstanceMemorySize() const
{
    return sizeof(float);
}

void UBTT_Rush::OnHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit)
{
    if (bHitDetected || !OtherActor || OtherActor == SelfActor)
        return;

    ECollisionChannel OtherChannel = Hit.Component->GetCollisionObjectType();

    if (OtherChannel == ECC_Pawn || OtherChannel == ECC_WorldStatic)
    {
        // Change to LogBTT
        UE_LOG(LogTemp, Warning, TEXT("Rush: OnHit: Valid hit with channel: %d"), OtherChannel);
        bHitDetected = true;


        /*
         *  If Door -> Ä«¿À½º·Î »Ñ¼Å¹ö¸®±â



         */
    }
}
