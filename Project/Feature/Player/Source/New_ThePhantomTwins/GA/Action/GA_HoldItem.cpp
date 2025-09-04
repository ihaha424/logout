#include "GA_HoldItem.h"
#include "Player/PlayerCharacter.h"
#include "Player/PS_Player.h"
#include "Objects/InventoryComponent.h"
#include "Objects/HeldItemComponent.h"
#include "Tags/TPTGameplayTags.h"
#include "Log/TPTLog.h"
#include "GameFramework/PlayerController.h"

UGA_HoldItem::UGA_HoldItem()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
    NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
    AbilityTags.AddTag(FTPTGameplayTags::Get().TPTGameplay_Character_State_HoldItem);
}

void UGA_HoldItem::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    APawn* Pawn = Cast<APawn>(ActorInfo->AvatarActor.Get());
    if (Pawn && !Pawn->IsLocallyControlled())
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
        return;
    }

    float SlotNumber = TriggerEventData ? TriggerEventData->EventMagnitude : -1.f;
    TPT_LOG(GALog, Warning, TEXT("SlotNumber %f"), SlotNumber);

    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    APlayerCharacter* Character = Cast<APlayerCharacter>(ActorInfo->AvatarActor.Get());
    if (!Character)
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
        return;
    }

    // HeldItemComponent 찾기
    UHeldItemComponent* HeldItemComp = Character->FindComponentByClass<UHeldItemComponent>();
    if (!HeldItemComp)
    {
        TPT_LOG(GALog, Warning, TEXT("ActivateAbility: HeldItemComponent를 찾을 수 없습니다"));
        EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
        return;
    }

    // ItemAbilityTable 설정 (필요한 경우)
    if (ItemAbilityTable && !HeldItemComp->ItemAbilityTable)
    {
        HeldItemComp->ItemAbilityTable = ItemAbilityTable;
    }

    // 선택된 아이템 타입 가져오기
    APlayerController* PlayerController = ActorInfo->PlayerController.Get();
    EItemType ChoiceItemType = EItemType::None;

    if (PlayerController)
    {
        APlayerState* PS = PlayerController->PlayerState;
        if (PS)
        {
            APS_Player* PlayerPS = Cast<APS_Player>(PS);
            if (PlayerPS)
            {
                UInventoryComponent* InventoryComponent = PlayerPS->InventoryComp;
                if (InventoryComponent)
                {
                    ChoiceItemType = InventoryComponent->ChoiceItem(static_cast<int32>(SlotNumber - 1));
                }
            }
        }
    }

    // HeldItemComponent를 통해 아이템 처리
    if (ChoiceItemType == EItemType::EMP || ChoiceItemType == EItemType::NoiseBomb || ChoiceItemType == EItemType::Key)
    {
        // 투척 아이템인 경우 스폰 및 부착
        HeldItemComp->SpawnAndAttachHeldItem(ChoiceItemType);

        TPT_LOG(GALog, Log, TEXT("투척 아이템 (%d) 손에 부착 완료"), static_cast<int32>(ChoiceItemType));

        // 여기서 포물선 인디케이터 출력 로직 추가 가능
    }
    else
    {
        // 투척 아이템이 아닌 경우 기존 아이템 제거
        HeldItemComp->DestroyHeldItem();

        TPT_LOG(GALog, Log, TEXT("투척 아이템이 아니므로 손에 있는 아이템 제거"));
    }

    EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
