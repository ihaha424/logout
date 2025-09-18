// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/ActiveSkill/GA_SceneAura.h"

#include "AbilitySystemComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "DrawDebugHelpers.h"
#include "Components/MeshComponent.h"
#include "Tags/TPTGameplayTags.h"
#include "Log/TPTLog.h"
#include "Components/PrimitiveComponent.h"
#include "Engine/OverlapResult.h"

UGA_SceneAura::UGA_SceneAura()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
    Sphere = FCollisionShape::MakeSphere(SenseRadius);
    AbilityTags.AddTag(FTPTGameplayTags::Get().TPTGameplay_InputTag_Player_ActiveSkill_E);
}

void UGA_SceneAura::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
    if (!Super::CommitAbility(Handle, ActorInfo, ActivationInfo))
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
        return;
    }
    UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();

    // 스킬 실행후 이 GA가 종료되는 시점을 정해줄 쿨타임 이펙트.
    FGameplayEffectSpecHandle CoolDownSpecHandle = MakeOutgoingGameplayEffectSpec(CoolDownEffect, 1.0f);
    if (CoolDownSpecHandle.IsValid())
    {
    	ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, CoolDownSpecHandle);
    }
    ASC->RegisterGameplayTagEvent(FTPTGameplayTags::Get().TPTGameplay_Character_State_OutLineCoolDown, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &ThisClass::OnCoolDownTagChanged);

    // 아우라가 실행되는 시간만큼 태그를 붙여줄 이펙트 실행. ( = 20초)
    FGameplayEffectSpecHandle SceneAuraSpecHandle = MakeOutgoingGameplayEffectSpec(SceneAuraEffect, 1.0f);
    if (SceneAuraSpecHandle.IsValid())
    {
        ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SceneAuraSpecHandle);
    }
    ASC->RegisterGameplayTagEvent(FTPTGameplayTags::Get().TPTGameplay_Character_State_UsingOutLine).AddUObject(this, &ThisClass::OnSceneAuraTagChanged);

    OwnerActor = GetAvatarActorFromActorInfo();
	NULLCHECK_RETURN_LOG(OwnerActor, GALog, Warning, )


    // 나에게서는 투영 안되게..
    // 문제는 이렇게 하면 나도 적용됨.. 벽 뒤에 가면 ㅠ
    TArray<UMeshComponent*> Meshes;
    OwnerActor->GetComponents<UMeshComponent>(Meshes);

    for (UMeshComponent* Mesh : Meshes)
    {
        if (!Mesh) continue;

        Mesh->SetCustomDepthStencilValue(0);
        Mesh->SetRenderCustomDepth(true);
    }

    // 첫 탐지 실행
    ScanTargets();

    // 주기적 탐지 시작
    GetWorld()->GetTimerManager().SetTimer(
        ScanTimerHandle,
        this,
        &UGA_SceneAura::ScanTargets,
        ScanInterval,
        true
    );
}

void UGA_SceneAura::ScanTargets()
{
    NULLCHECK_RETURN_LOG(OwnerActor, GALog, Warning, )

    FVector Origin = OwnerActor->GetActorLocation();
    TSet<TWeakObjectPtr<AActor>> NewTargets;
    UWorld* World = GetWorld();

    // 1) 상대 플레이어 & 특정 오브젝트
    {
        TArray<AActor*> UnlimitedObjects;
        UGameplayStatics::GetAllActorsWithTag(World, FName("AuraObject"), UnlimitedObjects);

		for (AActor* Target : UnlimitedObjects)
		{
			if (!Target || Target == OwnerActor) continue;

            if (IsValidAuraTarget(Target))
            {
				ApplyAuraToTarget(Target);
				NewTargets.Add(Target);
            }
        }
    }

    // 2) 적 
    {
        TArray<FOverlapResult> Overlaps;

        bool bHit = World->OverlapMultiByObjectType(
            Overlaps,
            Origin,
            FQuat::Identity,
            FCollisionObjectQueryParams(ECC_Pawn),
            Sphere
        );

        for (auto& Result : Overlaps)
        {
            AActor* Target = Result.GetActor();
            if (!Target || Target == OwnerActor) continue;

            if (Target->ActorHasTag("Enemy"))
            {
                FVector BoundsOrigin, BoundsExtent;
                Target->GetActorBounds(true, BoundsOrigin, BoundsExtent);

                // 몸체 전체가 Sphere 안에 들어왔는지 확인
                FVector MaxPoint = BoundsOrigin + BoundsExtent;
                FVector MinPoint = BoundsOrigin - BoundsExtent;

                if ((MaxPoint - Origin).Size() <= SenseRadius &&
                    (MinPoint - Origin).Size() <= SenseRadius)
                {
                    ApplyAuraToTarget(Target);
                    NewTargets.Add(Target);
                }
            }
        }
    }

    // 감지 안 된 대상 Aura 해제
    for (auto& OldTargetPtr : CurrentAuraTargets)
    {
        if (!NewTargets.Contains(OldTargetPtr))
        {
            if (AActor* OldTarget = OldTargetPtr.Get())
            {
                RemoveAuraFromTarget(OldTarget);
            }
        }
    }

    CurrentAuraTargets = NewTargets;
}

void UGA_SceneAura::ApplyAuraToTarget(AActor* Target)
{
	NULLCHECK_RETURN_LOG(Target, GALog, Warning, );
	NULLCHECK_RETURN_LOG(OwnerActor, GALog, Warning, );

	if (Target == OwnerActor) return;

    TArray<UMeshComponent*> Meshes;
    Target->GetComponents<UMeshComponent>(Meshes);

    for (UMeshComponent* Mesh : Meshes)
    {
        if (!Mesh) continue;

        Mesh->SetCustomDepthStencilValue(1);
        Mesh->SetRenderCustomDepth(true);
    }

	/*if (UMeshComponent* Mesh = Target->FindComponentByClass<UMeshComponent>())
	{
		Mesh->SetCustomDepthStencilValue(1);
		Mesh->SetRenderCustomDepth(true);
	}*/
}

void UGA_SceneAura::RemoveAuraFromTarget(AActor* Target)
{
    NULLCHECK_RETURN_LOG(Target, GALog, Warning, );

    TArray<UMeshComponent*> Meshes;
    Target->GetComponents<UMeshComponent>(Meshes);

    for (UMeshComponent* Mesh : Meshes)
    {
        if (!Mesh) continue;

        Mesh->SetRenderCustomDepth(false);
    }

	//if (UMeshComponent* Mesh = Target->FindComponentByClass<UMeshComponent>())
	//{
	//	Mesh->SetRenderCustomDepth(false);
	//}
}

bool UGA_SceneAura::IsValidAuraTarget(AActor* Target) const
{
    FVector Start = OwnerActor->GetActorLocation();
    FVector End = Target->GetActorLocation();

	FHitResult Hit;
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(OwnerActor); // 자기자신은 무시

	bool bHit = GetWorld()->LineTraceSingleByChannel(
	Hit,
	Start,
	End,
	ECollisionChannel::ECC_WorldStatic,
	Params
	);

	return Target != Hit.GetActor() ? true : false;
}

void UGA_SceneAura::OnSceneAuraTagChanged(const FGameplayTag InputTag, int32 TagCount)
{
    if (TagCount <= 0)
    {
        GetWorld()->GetTimerManager().ClearTimer(ScanTimerHandle);
        for (auto& TargetPtr : CurrentAuraTargets)
        {
            if (AActor* Target = TargetPtr.Get())
            {
                RemoveAuraFromTarget(Target);
            }
        }
        CurrentAuraTargets.Empty();
    }
}

void UGA_SceneAura::OnCoolDownTagChanged(const FGameplayTag InputTag, int32 TagCount)
{
    bHasCoolDownTag = TagCount > 0; // 태그가 붙었으면 true, 없으면 false
    if (!bHasCoolDownTag)
    {
        EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
    }
}
