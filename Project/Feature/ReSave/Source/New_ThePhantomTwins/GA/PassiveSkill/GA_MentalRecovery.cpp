// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_MentalRecovery.h"

#include "AbilitySystemComponent.h"
#include "Attribute/PlayerAttributeSet.h"
#include "Kismet/GameplayStatics.h"
#include "Log/TPTLog.h"
#include "Tags/TPTGameplayTags.h"
#include "Player/PlayerCharacter.h"

UGA_MentalRecovery::UGA_MentalRecovery()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor; 
    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;

    FGameplayTagContainer DefaultTags;
    DefaultTags.AddTag(FTPTGameplayTags::Get().TPTGameplay_Character_Skill_MentalRecovery);
    SetAssetTags(DefaultTags);
}

void UGA_MentalRecovery::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
    if (AvatarActor)
    {
        if (AvatarActor->HasAuthority()) // 서버에서만!
        {
            // 1초마다 반복
            GetWorld()->GetTimerManager().SetTimer(
                HealTimerHandle, this, &UGA_MentalRecovery::HealTick, 1.0f, true
            );
        }
    }
}

void UGA_MentalRecovery::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
    GetWorld()->GetTimerManager().ClearTimer(HealTimerHandle);
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_MentalRecovery::HealTick()
{
    TArray<AActor*> Players;
    UWorld* World = GetWorld();
    AActor* AvatarActor = GetAvatarActorFromActorInfo();
    NULLCHECK_RETURN_LOG(World, GALog, Error, );
    NULLCHECK_RETURN_LOG(AvatarActor, GALog, Error, );
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), GetAvatarActorFromActorInfo()->GetClass(), Players);

	TPT_LOG(GALog, Log, TEXT("UGA_MentalRecovery::HealTick - Players Num: %d"), Players.Num());

    if (Players.Num() == 2)
    {
        APlayerCharacter* SelfActor = Cast<APlayerCharacter>(GetAvatarActorFromActorInfo());
        APlayerCharacter* Other = (Cast<APlayerCharacter>(Players[0]) == SelfActor) ? Cast<APlayerCharacter>(Players[1]) : Cast<APlayerCharacter>(Players[0]);
        NULLCHECK_RETURN_LOG(SelfActor, GALog, Error, );
        NULLCHECK_RETURN_LOG(Other, GALog, Error, );

        float Dist = FVector::Dist(SelfActor->GetActorLocation(), Other->GetActorLocation());

        UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
        NULLCHECK_RETURN_LOG(ASC, GALog, Error, );

		UAbilitySystemComponent* OtherASC = Other->GetAbilitySystemComponent();
		NULLCHECK_RETURN_LOG(OtherASC, GALog, Error, );
        
        const auto* AttrSet = ASC->GetSet<UPlayerAttributeSet>();
        NULLCHECK_RETURN_LOG(AttrSet, GALog, Error, );

        //float CurrentMental = AttrSet->GetMentalPoint();
        //float MaxMental = AttrSet->GetMaxMentalPoint();


        // 둘다 쫓기는 상태가 아니면 회복
        if (!ASC->HasMatchingGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_State_AIChasing)
        && !OtherASC->HasMatchingGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_State_AIChasing))
        {

            TPT_LOG(GALog, Log, TEXT("UGA_MentalRecovery:: HAS NOT TPTGameplay_Character_State_AIChasing"));
            float HealAmount = 0.f;
            if (Dist <= 300.f)
                HealAmount = 5.f;

            FGameplayEffectSpecHandle MentalHealSpec = MakeOutgoingGameplayEffectSpec(MentalHealEffect, 1.0f);
            if (MentalHealSpec.IsValid())
            {
                MentalHealSpec.Data->SetSetByCallerMagnitude(FTPTGameplayTags::Get().TPTGameplay_Data_Effect_MentalPoint, HealAmount);
                GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToSelf(*MentalHealSpec.Data.Get());
            }
        }
    }
}
