// Fill out your copyright notice in the Description page of Project Settings.


#include "GC/GC_PlaySound.h"
#include "AbilitySystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Log/TPTLog.h"
#include "Tags/TPTGameplayTags.h"

AGC_PlaySound::AGC_PlaySound()
{
}

bool AGC_PlaySound::OnActive_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters)
{
    NULLCHECK_RETURN_LOG(MyTarget, GCLog, Error, false);
    // 1. 타겟에서 ASC(AbilitySystemComponent) 획득
    UAbilitySystemComponent* ASC = MyTarget->FindComponentByClass<UAbilitySystemComponent>();
    NULLCHECK_RETURN_LOG(ASC, GCLog, Error, false);
    // 태그가 있으면 지정된 사운드 재생 (본인한테만 재생, 멀티플레이 대응)
	APawn* Pawn = Cast<APawn>(MyTarget);
	if (!Pawn->IsLocallyControlled())
            return false;
    // 2. 태그 보유 여부 체크 및 분기
    if (SoundConfused1st && ASC->HasMatchingGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_State_Confused1st))
    {
        PlayTarget = SoundConfused1st;
    }
    else if (SoundConfused2nd && ASC->HasMatchingGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_State_Confused2nd))
    {
        PlayTarget = SoundConfused2nd;
    }
    else if (SoundConfused3rd && ASC->HasMatchingGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_State_Confused3rd))
    {
        PlayTarget = SoundConfused3rd;
    }

    UGameplayStatics::SpawnSoundAttached(PlayTarget, MyTarget->GetRootComponent());

    return true;
}
