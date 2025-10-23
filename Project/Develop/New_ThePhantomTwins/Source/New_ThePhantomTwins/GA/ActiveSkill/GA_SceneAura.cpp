// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/ActiveSkill/GA_SceneAura.h"

#include "AbilitySystemComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "DrawDebugHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/MeshComponent.h"
#include "Tags/TPTGameplayTags.h"
#include "Log/TPTLog.h"
#include "Components/PrimitiveComponent.h"
#include "Engine/OverlapResult.h"
#include "Player/PlayerCharacter.h"

UGA_SceneAura::UGA_SceneAura()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
    Sphere = FCollisionShape::MakeSphere(SenseRadius);

    FGameplayTagContainer DefaultTags;
    DefaultTags.AddTag(FTPTGameplayTags::Get().TPTGameplay_InputTag_Player_ActiveSkill_E);
    SetAssetTags(DefaultTags);
    ActivationBlockedTags.AddTag(FTPTGameplayTags::Get().TPTGameplay_Character_State_Hide);
}

bool UGA_SceneAura::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags,
    FGameplayTagContainer* OptionalRelevantTags) const
{
    bool bCanActivate = Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);

    if (!bCanActivate)
    {
        //UE_LOG(LogTemp, Warning, TEXT("CanActivateAbility failed - Cost or Cooldown or Tags check failed"));
        // 필요하다면 OptionalRelevantTags 안에 실패 원인을 분석 가능
       /* if (OptionalRelevantTags && OptionalRelevantTags->HasTag(FGameplayTag::RequestGameplayTag(FName("Ability.ActivateFail.Cost"))))
        {
            UE_LOG(LogTemp, Warning, TEXT("Activation failed due to Cost."));
        }*/

        APlayerCharacter* Avatar = Cast<APlayerCharacter>(ActorInfo->AvatarActor);
        if (Avatar && Avatar->IsLocallyControlled())
        {
            const_cast<UGA_SceneAura*>(this)->CallCoreEnergyZeroDialog(Avatar);
        }
    }
    return bCanActivate;
}

void UGA_SceneAura::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    if (!Super::CommitAbility(Handle, ActorInfo, ActivationInfo))
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
        return;
    }

    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

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
	NULLCHECK_RETURN_LOG(OwnerActor, GALog, Warning, );

    APlayerCharacter* Character = Cast<APlayerCharacter>(OwnerActor);

    if (Character && Character->IsLocallyControlled())
    {
        TArray<AActor*> Players;

        UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("PlayerAura"), Players);

        for (AActor* Player : Players)
        {
            if (Player && Player != OwnerActor)
            {
                OtherPlayer = Player;
                NewTargets.Add(OtherPlayer);
                ApplyAuraToTarget(OtherPlayer, 2);
            }
        }

        SpawnScanEffectActor();

        // 아우라 효과를 자기 자신에게도 적용 (초기설정)
        ApplyAuraToTarget(OwnerActor, 4);

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
}

void UGA_SceneAura::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);

    UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("AuraObject"), UnlimitedObjects);
}

void UGA_SceneAura::ScanTargets()
{
    NULLCHECK_RETURN_LOG(OwnerActor, GALog, Warning, )

    if (IsCameraBlocked())
    {
        RemoveAuraFromTarget(OwnerActor);
        //TPT_LOG(GALog, Log, TEXT("IsCameraBlocked"));
    }
    else
    {
        ApplyAuraToTarget(OwnerActor, 4);
    	//TPT_LOG(GALog, Log, TEXT("IsCameraNonBlocked"));
    }

    FVector Origin = OwnerActor->GetActorLocation();
    UWorld* World = GetWorld();

    // 1) 특정 오브젝트
    {
		for (AActor* Target : UnlimitedObjects)
		{
			if (!Target || Target == OwnerActor) continue;

            if (IsValid(Target) && IsValidAuraTarget(Target))
            {
				ApplyAuraToTarget(Target, 3);
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
                    ApplyAuraToTarget(Target, 1);
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

void UGA_SceneAura::ApplyAuraToTarget(AActor* Target, int32 Value)
{
	NULLCHECK_RETURN_LOG(Target, GALog, Warning, );
	NULLCHECK_RETURN_LOG(OwnerActor, GALog, Warning, );

    TArray<UMeshComponent*> Meshes;
    Target->GetComponents<UMeshComponent>(Meshes);

    for (UMeshComponent* Mesh : Meshes)
    {
        if (!Mesh) continue;

        Mesh->SetCustomDepthStencilValue(Value);
        Mesh->SetRenderCustomDepth(true);
    }
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

bool UGA_SceneAura::IsCameraBlocked()
{
    APlayerCharacter* Character = Cast<APlayerCharacter>(OwnerActor);
    NULLCHECK_RETURN_LOG(Character, GALog, Warning, true);

    FVector Start = Character->GetCamera()->GetComponentLocation();
    FVector End = OwnerActor->GetActorLocation();

    FHitResult Hit;
    FCollisionQueryParams Params;

    float SphereRadius = 40.0f;
    FCollisionShape TempSphere = FCollisionShape::MakeSphere(SphereRadius);

    bool bHit = GetWorld()->SweepSingleByChannel(
        Hit,
        Start,
        End,
        FQuat::Identity,
		ECC_Visibility,
		TempSphere,
        Params
    );

	// 맞았는데 자신인 경우에만 false 반환
    if (bHit && Hit.GetActor() == Character)
    {
        return false;
    }

    return true;
}

void UGA_SceneAura::SpawnScanEffectActor()
{
    UWorld* World = OwnerActor ? OwnerActor->GetWorld() : nullptr;
    if (!World) return;

    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = OwnerActor;
    SpawnParams.Instigator = OwnerActor->GetInstigator();
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    FVector SpawnLocation = OwnerActor->GetActorLocation();
    FRotator SpawnRotation = OwnerActor->GetActorRotation();

    if (ScanEffectActorClass)
    {
        AActor* EffectActor = World->SpawnActor<AActor>(
            ScanEffectActorClass,
            SpawnLocation,
            SpawnRotation,
            SpawnParams
        );
    }
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

		RemoveAuraFromTarget(OwnerActor);
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
