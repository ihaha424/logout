// Fill out your copyright notice in the Description page of Project Settings.


#include "GC/GC_PlaySound.h"
#include "AbilitySystemComponent.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Log/TPTLog.h"
#include "Player/PlayerCharacter.h"
#include "Tags/TPTGameplayTags.h"

AGC_PlaySound::AGC_PlaySound()
{
}

bool AGC_PlaySound::OnActive_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters)
{
    TPT_LOG(GCLog, Error, TEXT(""));
    NULLCHECK_RETURN_LOG(MyTarget, GCLog, Error, false);
    // 1. 타겟에서 ASC(AbilitySystemComponent) 획득
    UAbilitySystemComponent* ASC = Cast<APlayerCharacter>(MyTarget)->GetAbilitySystemComponent();
    NULLCHECK_RETURN_LOG(ASC, GCLog, Error, false);

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

    if (MyTarget->GetWorld()->GetFirstPlayerController()->GetPawn() == MyTarget)
    {
        ConfusedAudioComponent = UGameplayStatics::SpawnSoundAttached(PlayTarget, MyTarget->GetRootComponent());
    }

    return true;
}
bool AGC_PlaySound::OnRemove_Implementation(AActor* Target, const FGameplayCueParameters& Parameters)
{
    if (ConfusedAudioComponent && ConfusedAudioComponent->IsPlaying())
    {
        ConfusedAudioComponent->Stop();
        ConfusedAudioComponent = nullptr;
    }
    return true;
}