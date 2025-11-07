// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_Rush.h"
#include "Behaviortree/Blackboardcomponent.h"
#include "AIController.h"
#include "AI/Character/AIBaseCharacter.h"
#include "Components/ShapeComponent.h"
#include "Components/CapsuleComponent.h"
#include "SzInterface/Destroyable.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "Log/TPTLog.h"

struct FCurrentSpeed : FBaseTaskNodeMemory
{
    float CurrentSpeed;
};

UBTT_Rush::UBTT_Rush()
{
	NodeName = TEXT("Rush");
	bNotifyTick = true;
	bNotifyTaskFinished = true;
}

EBTNodeResult::Type UBTT_Rush::Execute_Task(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();;
    NULLCHECK_RETURN_LOG(BlackboardComp, AILog, Warning, EBTNodeResult::Failed);

    ACharacter* Target = Cast<ACharacter>(BlackboardComp->GetValueAsObject(TargetActorKey.SelectedKeyName));
    NULLCHECK_RETURN_LOG(Target, AILog, Warning, EBTNodeResult::Failed);

    APawn* Pawn = OwnerComp.GetAIOwner()->GetPawn();
    NULLCHECK_RETURN_LOG(Pawn, AILog, Warning, EBTNodeResult::Failed);

    AAIBaseCharacter* AIBaseCharacter = Cast<AAIBaseCharacter>(Pawn);
    NULLCHECK_RETURN_LOG(AIBaseCharacter, AILog, Warning, EBTNodeResult::Failed);

    AAIController* AIController = OwnerComp.GetAIOwner();
    NULLCHECK_RETURN_LOG(AIController, AILog, Warning, EBTNodeResult::Failed);

    AIBaseCharacter->SetAttackCollision(true);
    AIBaseCharacter->GetAttackCollision().SetCollisionResponseToChannel(ECC_Visibility, ECR_Overlap);
    AIBaseCharacter->GetAttackCollision().SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap);
    AIBaseCharacter->GetAttackCollision().SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
    AIBaseCharacter->GetAttackCollision().OnComponentBeginOverlap.AddDynamic(this, &UBTT_Rush::OnHit);

    RushDirection = Target->GetActorLocation() - Pawn->GetActorLocation();
    RushDirection.Normalize();

    CachedOwnerComp = &OwnerComp;
    CurrentSpeedPtr = &((FCurrentSpeed*)NodeMemory)->CurrentSpeed;
    *CurrentSpeedPtr = InitialSpeed;


    UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
    NULLCHECK_RETURN_LOG(BB, AILog, Warning, EBTNodeResult::Failed);

    BB->SetValueAsBool(HitDetectedKey.SelectedKeyName, false);
    AIController->StopMovement();

    return EBTNodeResult::InProgress;
}

void UBTT_Rush::Execute_TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
    UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
    NULLCHECK_RETURN_LOG(BB, AILog, Warning, );
    AAIController* AIController = OwnerComp.GetAIOwner();
    NULLCHECK_RETURN_LOG(AIController, AILog, Warning, );

    bool bHitDetected = BB->GetValueAsBool(HitDetectedKey.SelectedKeyName);
    if (bHitDetected)
    {
        FinishLatentTask(*CachedOwnerComp, EBTNodeResult::Succeeded);
        AIController->StopMovement();
        return;
    }


    APawn* Pawn = AIController->GetPawn();
    NULLCHECK_RETURN_LOG(Pawn, AILog, Warning, );
    ACharacter* Character = Cast<ACharacter>(Pawn);
    NULLCHECK_RETURN_LOG(Character, AILog, Warning, );

    float& CurrentSpeed = ((FCurrentSpeed*)NodeMemory)->CurrentSpeed;
    CurrentSpeed += Acceleration * DeltaSeconds;

    Character->GetCharacterMovement()->MaxWalkSpeed = MaxSpeed;
    Character->AddMovementInput(RushDirection, CurrentSpeed * DeltaSeconds);
}

void UBTT_Rush::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
    APawn* Pawn = OwnerComp.GetAIOwner()->GetPawn();
    NULLCHECK_RETURN_LOG(Pawn, AILog, Warning, );

    AAIBaseCharacter* AIBaseCharacter = Cast<AAIBaseCharacter>(Pawn);
    NULLCHECK_RETURN_LOG(AIBaseCharacter, AILog, Warning, );

    AIBaseCharacter->SetAttackCollision(false);
    AIBaseCharacter->GetAttackCollision().OnComponentBeginOverlap.RemoveDynamic(this, &UBTT_Rush::OnHit);

    UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
    NULLCHECK_RETURN_LOG(BB, AILog, Warning, );

    BB->SetValueAsBool(StunKey.SelectedKeyName, true);
}

FString UBTT_Rush::GetStaticDescription() const
{
	return FString("Rush Task");
}

uint16 UBTT_Rush::GetInstanceMemorySize() const
{
    return sizeof(FCurrentSpeed);
}

void UBTT_Rush::OnHit(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    AActor* thisActor = OverlappedComp->GetOwner();
    NULLCHECK_RETURN_LOG(thisActor, AILog, Error, );
    APawn* thisPawn = Cast<APawn>(thisActor);
    NULLCHECK_RETURN_LOG(thisPawn, AILog, Error, );
    AController* thisController = thisPawn->GetController();
    NULLCHECK_RETURN_LOG(thisController, AILog, Error, );
    AAIController* thisAIController = Cast<AAIController>(thisController);
    NULLCHECK_RETURN_LOG(thisAIController, AILog, Error, );
    UBlackboardComponent* BB = thisAIController->GetBlackboardComponent();
    NULLCHECK_RETURN_LOG(BB, AILog, Warning, );

    bool bHitDetected = BB->GetValueAsBool(HitDetectedKey.SelectedKeyName);
    if (bHitDetected || !OtherActor || OtherActor == thisActor)
        return;

    ECollisionChannel OtherChannel = SweepResult.Component->GetCollisionObjectType();
    if (OtherChannel == ECC_Pawn || OtherChannel == ECC_WorldStatic || OtherChannel == ECC_WorldDynamic) // TODO: 플레이어 케릭터 또는 오브젝트로 변환햐여함.
    {
        AActor* Target = OtherActor;
        if (Target && Target->GetClass()->ImplementsInterface(UDestroyable::StaticClass()))
        {
            if (IDestroyable::Execute_CanBeDestroyed(Target, Cast<APawn>(thisActor)))
            {
                IDestroyable::Execute_OnDestroy(Target, Cast<APawn>(thisActor));
            }
        }
        ACharacter* TargetChar = Cast<ACharacter>(OtherActor);
        if (IsValid(TargetChar) && OtherComp != TargetChar->GetCapsuleComponent())
        {
            return;
        }
        BB->SetValueAsBool(HitDetectedKey.SelectedKeyName, true);
    }
}
