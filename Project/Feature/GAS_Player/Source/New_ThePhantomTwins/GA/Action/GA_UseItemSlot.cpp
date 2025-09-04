// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_UseItemSlot.h"
#include "Objects/InventoryComponent.h"
#include "Player/PS_Player.h"
#include "Player/PC_Player.h"
#include "Gameframework/PlayerController.h"
#include "Gameframework/PlayerState.h"

#include "Log/TPTLog.h"


UGA_UseItemSlot::UGA_UseItemSlot()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;
}

void UGA_UseItemSlot::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
    NULLCHECK_RETURN_LOG(TriggerEventData, GALog, Error, );

    float SlotNumber = TriggerEventData->EventMagnitude;
    //TPT_LOG(HUDLog, Warning, TEXT(" %f"), SlotNumber);

    // 이 GA를 부른 플레이어의 PlayerState에 있는 인벤토리Component에 접근해서 UseItem 호출
    
    // ActorInfo를 통해 PlayerController에 접근
    APlayerController* PlayerController = ActorInfo->PlayerController.Get();
    if (PlayerController)
    {
        // PlayerController에서 PlayerState 가져오기
        APlayerState* PS = PlayerController->PlayerState;

        if (PS)
        {
            // 커스텀 PlayerState로 캐스팅 (프로젝트에 맞게 수정 필요)
            APS_Player* PlayerPS = Cast<APS_Player>(PS);
            if (PlayerPS)
            {
                // 인벤토리 컴포넌트 가져오기
                UInventoryComponent* InventoryComponent = PlayerPS->InventoryComp;

                if (InventoryComponent)
                {
                    // UseItem 호출
                    InventoryComponent->UseItem(static_cast<int32>(SlotNumber));
                }
            }
        }
    }

    EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
