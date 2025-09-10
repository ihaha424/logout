// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/ActiveSkill/GA_SceneAura.h"

#include "AbilitySystemComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "DrawDebugHelpers.h"
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
    TPT_LOG(GALog, Warning, TEXT("Scan Aura!"));
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

    // 첫 탐지 실행
    ScanTargets();

}

void UGA_SceneAura::ScanTargets()
{
    AActor* Owner = GetAvatarActorFromActorInfo();
    NULLCHECK_RETURN_LOG(Owner, GALog, Warning, )

    FVector Origin = Owner->GetActorLocation();
    TSet<TWeakObjectPtr<AActor>> NewTargets;
    UWorld* World = GetWorld();


    // 1) 상대 플레이어 & 특정 오브젝트
    {
        TArray<AActor*> UnlimitedObjects;
        UGameplayStatics::GetAllActorsWithTag(World, FName("AuraObject"), UnlimitedObjects);

        for (AActor* Target : UnlimitedObjects)
        {
            if (!Target || Target == Owner) continue;
            ApplyAuraToTarget(Target);
            NewTargets.Add(Target);
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
            if (!Target || Target == Owner) continue;

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

	if (UMeshComponent* Mesh = Target->FindComponentByClass<UMeshComponent>())
	{
        TPT_LOG(GALog, Warning, TEXT("See Aura"));
		Mesh->SetRenderCustomDepth(true);
		Mesh->SetCustomDepthStencilValue(1);
	}
}

void UGA_SceneAura::RemoveAuraFromTarget(AActor* Target)
{
    NULLCHECK_RETURN_LOG(Target, GALog, Warning, );

	if (UMeshComponent* Mesh = Target->FindComponentByClass<UMeshComponent>())
	{
		Mesh->SetRenderCustomDepth(false);
	}
}

void UGA_SceneAura::OnSceneAuraTagChanged(const FGameplayTag InputTag, int32 TagCount)
{
    if (TagCount > 0)
    {
        // 주기적 탐지 시작
        GetWorld()->GetTimerManager().SetTimer(
            ScanTimerHandle,
            this,
            &UGA_SceneAura::ScanTargets,
            ScanInterval,
            true
        );
    }
    else
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
