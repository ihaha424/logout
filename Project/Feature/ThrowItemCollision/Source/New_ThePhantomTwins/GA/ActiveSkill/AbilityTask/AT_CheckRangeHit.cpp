// Fill out your copyright notice in the Description page of Project Settings.


#include "AT_CheckRangeHit.h"

#include "AbilitySystemComponent.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "GA/ActiveSkill/TargetActor/TA_RangeHit.h"
#include "Log/TPTLog.h"

UAT_CheckRangeHit::UAT_CheckRangeHit()
{
}

UAT_CheckRangeHit* UAT_CheckRangeHit::CreateTask(UGameplayAbility* OwningAbility, TSubclassOf<class ATA_RangeHit> TargetActorClass)
{
	UAT_CheckRangeHit* NewTask = NewAbilityTask<UAT_CheckRangeHit>(OwningAbility);
	NewTask->TargetActorClass = TargetActorClass;
	return NewTask;
}

void UAT_CheckRangeHit::Activate()
{
	Super::Activate();
	SpawnAndInitializeTargetActor();
	FinalizeTargetActor();

	SetWaitingOnAvatar();
}

void UAT_CheckRangeHit::OnDestroy(bool bAbilityEnded)
{
	if (SpawnedTargetActor)
	{
		SpawnedTargetActor->Destroy();
	}
	Super::OnDestroy(bAbilityEnded);
}

void UAT_CheckRangeHit::SpawnAndInitializeTargetActor()
{
	SpawnedTargetActor = Cast<ATA_RangeHit>(Ability->GetWorld()->SpawnActorDeferred<AGameplayAbilityTargetActor>(TargetActorClass, FTransform::Identity, nullptr, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn));
	if (SpawnedTargetActor)
	{
		SpawnedTargetActor->SetShowDebug(true);
		//나중에 타겟 데이터가 준비되었을 때 호출될 함수를 등록하는 단계입니다.
		SpawnedTargetActor->TargetDataReadyDelegate.AddUObject(this, &UAT_CheckRangeHit::OnTargetDataReadyCallback);
	}
}

void UAT_CheckRangeHit::FinalizeTargetActor()
{
	UAbilitySystemComponent* ASC = AbilitySystemComponent.Get();
	NULLCHECK_RETURN_LOG(ASC, TaskLog, Error, );

	const FTransform SpawnTransform = ASC->GetAvatarActor()->GetTransform();
	SpawnedTargetActor->FinishSpawning(SpawnTransform);
	ASC->SpawnedTargetActors.Push(SpawnedTargetActor);
	SpawnedTargetActor->StartTargeting(Ability);
	SpawnedTargetActor->ConfirmTargeting();
}

void UAT_CheckRangeHit::OnTargetDataReadyCallback(const FGameplayAbilityTargetDataHandle& DataHandle)
{
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnComplete.Broadcast(DataHandle);
	}
	//이제 여기에서 할일을 마무리 해서 브로드캐스트를 하는건데, 잊지말고 GA에서도 마루리 되게끔 해줘야한다.
	EndTask();
}
