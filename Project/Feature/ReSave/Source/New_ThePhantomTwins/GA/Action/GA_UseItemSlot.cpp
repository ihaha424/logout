// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_UseItemSlot.h"
#include "Objects/InventoryComponent.h"
#include "Player/PS_Player.h"
#include "Gameframework/PlayerController.h"
#include "Gameframework/PlayerState.h"
#include "Tags/TPTGameplayTags.h"
#include "Log/TPTLog.h"


UGA_UseItemSlot::UGA_UseItemSlot()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;

    FGameplayTagContainer DefaultTags;
    DefaultTags.AddTag(FTPTGameplayTags::Get().TPTGameplay_InputTag_Player_UseItem);
    SetAssetTags(DefaultTags);
}

void UGA_UseItemSlot::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
    NULLCHECK_RETURN_LOG(TriggerEventData, GALog, Error, );

    float SlotNumber = TriggerEventData->EventMagnitude;

    // 이 GA를 부른 플레이어의 PlayerState에 있는 인벤토리Component에 접근해서 UseItem 호출
    APlayerController* PlayerController = ActorInfo->PlayerController.Get();
    NULLCHECK_CODE_RETURN_LOG(PlayerController, GALog, Warning, EndAbility(Handle, ActorInfo, ActivationInfo, true, false);, );

    APS_Player* PS = Cast<APS_Player>(PlayerController->PlayerState);
	NULLCHECK_CODE_RETURN_LOG(PlayerController, GALog, Warning, EndAbility(Handle, ActorInfo, ActivationInfo, true, false);, );

    UInventoryComponent* InventoryComponent = PS->InventoryComp;
    NULLCHECK_CODE_RETURN_LOG(PlayerController, GALog, Warning, EndAbility(Handle, ActorInfo, ActivationInfo, true, false);, );
    {
        // UseItem 호출
        InventoryComponent->UseItem(static_cast<int32>(SlotNumber));
    }
    EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
