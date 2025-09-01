// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Character/AIScanner.h"
#include "AbilitySystemComponent.h"
#include "Tags/TPTGameplayTags.h"
#include "AbilitySystemGlobals.h"
#include "Log/TPTLog.h"

#include "Components/SpotLightComponent.h"
#include "Engine/SpotLight.h"
#include "Engine/OverlapResult.h"

bool AAIScanner::MatchingChaseActorType(AActor* OtherActor) const
{
    UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OtherActor);
    if (nullptr == ASC)
        return false;

    return ASC->HasMatchingGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_Identifier_Player) || ASC->HasMatchingGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_Identifier_AI);
}

void AAIScanner::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    //UpdateNearbySpotLights();
}

// 틱 또는 타이머로 주기 갱신
void AAIScanner::UpdateNearbySpotLights()
{
    UWorld* World = GetWorld();
    if (!World) return;

    TArray<FOverlapResult> Hits;
    FCollisionShape Sphere = FCollisionShape::MakeSphere(LightAffectRadius);

    FCollisionQueryParams Params(SCENE_QUERY_STAT(LightSweep), false, this);
    World->OverlapMultiByObjectType(
        Hits,
        GetActorLocation(),
        FQuat::Identity,
        FCollisionObjectQueryParams(ECC_WorldStatic | ECC_WorldDynamic | ECC_Visibility),
        Sphere,
        Params
    );

    // 이번 프레임에 잡힌 스포트라이트 집합
    TSet<ULightComponent*> Current;
    for (const auto& H : Hits)
    {
        if (ULightComponent* Comp = H.GetActor()->FindComponentByClass<ULightComponent>())
        {
            Current.Add(Comp);

            if (!ManagedLights.Find(Comp))
            {
                FLightState S;
                S.OriginalIntensity = Comp ? Comp->Intensity : 0.f;
                S.OverlapCount = 0;
                ManagedLights.Add(Comp, S);
            }
            if (Comp && Comp->IsVisible())
            {
                ManagedLights[Comp].OverlapCount++;
                // 끌 때: Visibility Off 또는 Intensity 0 중 택1
                Comp->SetVisibility(false);
                // Comp->SetIntensity(0.f);
            }
        }
    }

    // 이번 프레임에 안 잡힌 라이트는 카운트 -1
    for (auto& Pair : ManagedLights)
    {
        ULightComponent* L = Pair.Key.Get();
        if (!L) continue;

        if (!Current.Contains(L))
        {
            Pair.Value.OverlapCount = FMath::Max(0, Pair.Value.OverlapCount - 1);
            if (Pair.Value.OverlapCount == 0)
            {
                
                L->SetVisibility(true);
                //L->SetIntensity(Pair.Value.OriginalIntensity);
                
            }
        }
    }

    // GC된 라이트 정리(옵션)
    for (auto It = ManagedLights.CreateIterator(); It; ++It)
    {
        if (!It.Key().IsValid())
            It.RemoveCurrent();
    }
}
