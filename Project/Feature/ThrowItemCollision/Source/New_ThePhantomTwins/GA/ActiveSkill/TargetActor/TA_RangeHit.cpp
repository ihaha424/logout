// Fill out your copyright notice in the Description page of Project Settings.


#include "TA_RangeHit.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "AbilitySystemComponent.h"
#include "Engine/OverlapResult.h"

ATA_RangeHit::ATA_RangeHit()
{
	Radius = 300.f;
}

void ATA_RangeHit::StartTargeting(UGameplayAbility* InAbility)
{
    Super::StartTargeting(InAbility);

}

void ATA_RangeHit::ConfirmTargetingAndContinue()
{
    // 컨펌이 확정되고 실행되는 구문.
    if (SourceActor)
    {
        FGameplayAbilityTargetDataHandle DataHandle = MakeTargetData();
        TargetDataReadyDelegate.Broadcast(DataHandle);
    }
}

FGameplayAbilityTargetDataHandle ATA_RangeHit::MakeTargetData() const
{
    TArray<TWeakObjectPtr<AActor>> HitActors;

    // 충돌 쿼리 파라미터(나 자신 제외)
    FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(AbilityTargeting), false, GetOwner());

    TArray<FOverlapResult> Overlaps;
	GetWorld()->OverlapMultiByObjectType(Overlaps,GetActorLocation(),FQuat::Identity,FCollisionObjectQueryParams(ECC_Pawn), // 클리치 함정을 잠시 끄려면..?
            FCollisionShape::MakeSphere(Radius),QueryParams);

	for (const FOverlapResult& Result : Overlaps)
    {
        AActor* Actor = Result.GetActor();
        if (!Actor) continue;

        AActor* HitActor = Result.OverlapObjectHandle.FetchActor<AActor>();
        if (HitActor && !HitActors.Contains(HitActor)&& Actor->Tags.Contains(FName(TEXT("Enemy"))))// 클리치 함정을 잠시 끄려면..?
        {
            HitActors.Add(HitActor);
        }
    }

    FGameplayAbilityTargetData_ActorArray* ActorsData = new FGameplayAbilityTargetData_ActorArray();
    ActorsData->SetActors(HitActors);

#if ENABLE_DRAW_DEBUG

    if (bShowDebug)
    {
        FColor DrawColor = HitActors.Num() > 0 ? FColor::Green : FColor::Red;
        DrawDebugSphere(GetWorld(), GetActorLocation(), Radius, 16, DrawColor, false, 5.0f);
    }

#endif
    return FGameplayAbilityTargetDataHandle(ActorsData);
}
