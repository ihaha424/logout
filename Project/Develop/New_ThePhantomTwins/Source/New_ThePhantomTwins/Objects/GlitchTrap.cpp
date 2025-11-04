
#include "GlitchTrap.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h"

AGlitchTrap::AGlitchTrap() : AOverlapObject()
{
	ObjectTag = FName("GlitchTrap");
}

void AGlitchTrap::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
    Super::EndPlay(EndPlayReason);
}


void AGlitchTrap::ActivateMentalDamage()
{
    // 서버 전용 처리
    //if (!HasAuthority()) return;

    bEnableEffectAndCue = true;

    if (!BoxTrigger) return;

    TArray<AActor*> OverlappingActors;
    BoxTrigger->GetOverlappingActors(OverlappingActors);

    for (AActor* Actor : OverlappingActors)
    {
        if (!Actor) continue;

        // 이미 적용되어 있으면 스킵
        if (ActiveEffectHandles.Contains(Actor))
        {
            // 이미 적용된 경우에도 Cue가 없을 수 있으니 ASC 체크해서 Cue 강제 실행 가능
            UAbilitySystemComponent* ExistingASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Actor);
            if (ExistingASC)
            {
                FGameplayCueParameters Param;
                Param.SourceObject = this;
                Param.Instigator = Actor;
                Param.Location = GetActorLocation();
                ExistingASC->ExecuteGameplayCue(GameplayCueTag, Param);
            }
            ReceiveBeginOverlap(Actor);
            continue;
        }

        UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Actor);
        if (TargetASC && GameplayEffectClass)
        {
            // 1) 지속형 GE 적용 및 핸들 저장
            FGameplayEffectContextHandle EffectContext = TargetASC->MakeEffectContext();
            EffectContext.AddSourceObject(this);

            FGameplayEffectSpecHandle SpecHandle = TargetASC->MakeOutgoingSpec(GameplayEffectClass, 1.0f, EffectContext);
            if (SpecHandle.IsValid() && SpecHandle.Data.IsValid())
            {
                FActiveGameplayEffectHandle ActiveHandle = TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
                if (ActiveHandle.IsValid())
                {
                    ActiveEffectHandles.Add(Actor, ActiveHandle);
                }
            }

            // 2) 지속형 Cue 실행
            FGameplayCueParameters Param;
            Param.SourceObject = this;
            Param.Instigator = Actor;
            Param.Location = GetActorLocation();
            TargetASC->ExecuteGameplayCue(GameplayCueTag, Param);
        }

        // 블루프린트용 콜백 (플레이어에게 상태가 시작되었음을 알림)
        ReceiveBeginOverlap(Actor);
    }

    Activate();
}

void AGlitchTrap::DeactivateMentalDamage(float DisableDuration)
{
    // 서버 전용 처리
    //if (!HasAuthority()) return;

    bEnableEffectAndCue = false;

    if (!BoxTrigger) return;

    TArray<AActor*> OverlappingActors;
    BoxTrigger->GetOverlappingActors(OverlappingActors);

    for (AActor* Actor : OverlappingActors)
    {
        if (!Actor) continue;

        UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Actor);
        if (TargetASC)
        {
            // 1) 적용된 GE 제거 (있으면)
            if (ActiveEffectHandles.Contains(Actor))
            {
                FActiveGameplayEffectHandle Handle = ActiveEffectHandles[Actor];
                if (Handle.IsValid())
                {
                    TargetASC->RemoveActiveGameplayEffect(Handle);
                }
                ActiveEffectHandles.Remove(Actor);
            }

            // 2) 지속형 Cue 제거
            TargetASC->RemoveGameplayCue(GameplayCueTag);
        }

        // 블루프린트용 콜백 (플레이어에게서 상태가 끝났음을 알림)
        ReceiveEndOverlap(Actor);
    }

    Deactivate(DisableDuration);

    // 일정 시간 뒤 다시 true로 복원
    GetWorld()->GetTimerManager().SetTimer(
        TimerHandle,
        this,
        &AGlitchTrap::ActivateMentalDamage,
        DisableDuration,
        false
    );
}