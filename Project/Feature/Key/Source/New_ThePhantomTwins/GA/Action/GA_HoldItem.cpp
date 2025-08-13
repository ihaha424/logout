// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_HoldItem.h"

#include "Player/PlayerCharacter.h"
#include "Player/PS_Player.h"
#include "Player/PC_Player.h"
#include "Objects/InventoryComponent.h"
#include "Tags/TPTGameplayTags.h"
#include "Log/TPTLog.h"

UGA_HoldItem::UGA_HoldItem()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;
	AbilityTags.AddTag(FTPTGameplayTags::Get().TPTGameplay_Character_State_HoldItem);
}

void UGA_HoldItem::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    TPT_LOG(GALog, Log, TEXT("UGA_HoldItem::ActivateAbility"));


	APlayerCharacter* Character = Cast<APlayerCharacter>(ActorInfo->AvatarActor.Get());
	if (!Character) return;

	USkeletalMeshComponent* MeshComp = Character->GetMesh();
	FName HandSocketName = TEXT("Hand_RSocket"); //  // 손 소켓을 설정해야함.  ////

	// 캐릭터가 선택한 아이템을 받아와야함.
	// 손에 아이템을 붙이기.
	// 손에서 충돌판정이 안되도록 하기.

	// UGA_HoldItemㅡ 이 GA에 해당하는 태그가 붙어있는 상태면 손에 뭘 집고 있는 상태의 애니메이션이 출력되도록 하기.


    float SlotNumber = TriggerEventData->EventMagnitude;

    // 이 GA를 부른 플레이어의 PlayerState에 있는 인벤토리Component에 접근해서 UseItem 호출



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
                    // ChoiceItem 호출(인벤토리 슬롯 아웃라인 및 아이템 툴팁 설정)
                    InventoryComponent->ChoiceItem(static_cast<int32>(SlotNumber));
                }
            }
        }
    }

}
