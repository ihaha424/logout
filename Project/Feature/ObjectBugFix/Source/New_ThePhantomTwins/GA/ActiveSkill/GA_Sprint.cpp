// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_Sprint.h"
#include "AbilitySystemComponent.h"
#include "Log/TPTLog.h"
#include "Tags/TPTGameplayTags.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"
#include "Player/PlayerCharacter.h"

UGA_Sprint::UGA_Sprint()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;

    FGameplayTagContainer DefaultTags;
    DefaultTags.AddTag(FTPTGameplayTags::Get().TPTGameplay_InputTag_Player_ActiveSkill_Q);
    SetAssetTags(DefaultTags);

    ActivationBlockedTags.AddTag(FTPTGameplayTags::Get().TPTGameplay_Character_State_Hide);
}

bool UGA_Sprint::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
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

        if (AActor* Owner = Cast<AActor>(ActorInfo->AvatarActor))
        {
            if (APlayerController* PlayerController = Cast<APlayerController>(Owner->GetInstigatorController()))
            {
                const_cast<UGA_Sprint*>(this)->S2C_ShowZeroEnergyDialog(PlayerController);
            }
        }
    }
    return bCanActivate;
}

void UGA_Sprint::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, 
    const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    if (!Super::CommitAbility(Handle, ActorInfo, ActivationInfo))
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
        return;
    }

    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
    UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();

    APlayerCharacter* Character = Cast<APlayerCharacter>(ActorInfo->AvatarActor.Get());

    if (Character && Character->IsLocallyControlled())
    {
        if (SoundCue) // SoundCue는 클래스에 UPROPERTY로 선언되어 있어야 함
        {
            ActiveAudioComponent = UGameplayStatics::SpawnSoundAttached(SoundCue, Character->GetRootComponent());
        }
    }

    // 스프린트가 실행되는 시간만큼 태그를 붙여줄 이펙트 실행. ( = 5초)
    FGameplayEffectSpecHandle SprintSpecHandle = MakeOutgoingGameplayEffectSpec(SprintEffect, 1.0f);
    if (SprintSpecHandle.IsValid())
    {
        ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SprintSpecHandle);
    }
    // 이 스프린트 GA가 종료될 시점을 알려줄 쿨타임 이펙트 실행.
    FGameplayEffectSpecHandle CoolDownSpecHandle = MakeOutgoingGameplayEffectSpec(CoolDownEffect, 1.0f);
    if (CoolDownSpecHandle.IsValid())
    {
        ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, CoolDownSpecHandle);
    }
    ASC->RegisterGameplayTagEvent(FTPTGameplayTags::Get().TPTGameplay_Character_State_SprintCoolDown, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &ThisClass::OnCoolDownTagChanged);
}

void UGA_Sprint::OnCoolDownTagChanged(const FGameplayTag InputTag, int32 TagCount)
{
    bHasCoolDownTag = TagCount > 0; // 태그가 붙었으면 true, 없으면 false
    if (!bHasCoolDownTag)
    {
        if (ActiveAudioComponent)
        {
            ActiveAudioComponent->Stop();
            ActiveAudioComponent = nullptr;
        }

        EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
    }
}

void UGA_Sprint::S2C_ShowZeroEnergyDialog_Implementation(APlayerController* PC)
{
    CallCoreEnergyZeroDialog(PC);
}
