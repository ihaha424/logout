// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_LookAtLocation.h"
#include "AI/AIBaseAnimInstance.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Log/TPTLog.h"

UBTT_LookAtLocation::UBTT_LookAtLocation()
{
	NodeName = TEXT("LookAtLocation");
}

EBTNodeResult::Type UBTT_LookAtLocation::Execute_Task(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	NULLCHECK_RETURN_LOG(AIController, AILog, Warning, EBTNodeResult::Failed);
	APawn* AIPawn = AIController->GetPawn();
	NULLCHECK_RETURN_LOG(AIPawn, AILog, Warning, EBTNodeResult::Failed);
	ACharacter* Character = Cast<ACharacter>(AIPawn);
	NULLCHECK_RETURN_LOG(Character, AILog, Warning, EBTNodeResult::Failed);
	USkeletalMeshComponent* SkeletalMeshComponent = Character->GetMesh();
	NULLCHECK_RETURN_LOG(SkeletalMeshComponent, AILog, Warning, EBTNodeResult::Failed);
	UAnimInstance* AnimInstance = SkeletalMeshComponent->GetAnimInstance();
	NULLCHECK_RETURN_LOG(AnimInstance, AILog, Warning, EBTNodeResult::Failed);
	UAIBaseAnimInstance* AIBaseAnimInstance = Cast<UAIBaseAnimInstance>(AnimInstance);
	NULLCHECK_RETURN_LOG(AIBaseAnimInstance, AILog, Warning, EBTNodeResult::Failed);

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	NULLCHECK_RETURN_LOG(BB, AILog, Warning, EBTNodeResult::Failed);
	AActor* TargetActor = Cast<AActor>(BB->GetValueAsObject(TargetActorKey.SelectedKeyName));
	NULLCHECK_RETURN_LOG(TargetActor, AILog, Warning, EBTNodeResult::Failed);

	AIBaseAnimInstance->SetLookAtTargetActor(TargetActor);
	return EBTNodeResult::Succeeded;
}

FString UBTT_LookAtLocation::GetStaticDescription() const
{
	return FString("Look at the target Actor.");
}

void UBTT_LookAtLocation::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	//TODO: State 초기화 할때 TargetActor도 초기화 해야함. 추격상태면 계속봐야하니까

	//AAIController* AIController = OwnerComp.GetAIOwner();
	//NULLCHECK_RETURN_LOG(AIController, AILog, Warning,);
	//APawn* AIPawn = AIController->GetPawn();
	//NULLCHECK_RETURN_LOG(AIPawn, AILog, Warning,);
	//ACharacter* Character = Cast<ACharacter>(AIPawn);
	//NULLCHECK_RETURN_LOG(Character, AILog, Warning,);
	//USkeletalMeshComponent* SkeletalMeshComponent = Character->GetMesh();
	//NULLCHECK_RETURN_LOG(SkeletalMeshComponent, AILog, Warning,);
	//UAnimInstance* AnimInstance = SkeletalMeshComponent->GetAnimInstance();
	//NULLCHECK_RETURN_LOG(AnimInstance, AILog, Warning,);
	//UAIBaseAnimInstance* AIBaseAnimInstance = Cast<UAIBaseAnimInstance>(AnimInstance);
	//NULLCHECK_RETURN_LOG(AIBaseAnimInstance, AILog, Warning,);

	//UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	//NULLCHECK_RETURN_LOG(BB, AILog, Warning,);
	//AActor* TargetActor = Cast<AActor>(BB->GetValueAsObject(TargetActorKey.SelectedKeyName));
	//NULLCHECK_RETURN_LOG(TargetActor, AILog, Warning,);

	//AIBaseAnimInstance->SetLookAtTargetActor(nullptr);
	//EBTNodeResult::Succeeded;
}
