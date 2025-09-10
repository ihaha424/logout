#include "GA_HoldItem.h"

#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
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
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    APawn* Pawn = Cast<APawn>(ActorInfo->AvatarActor.Get());
    if (Pawn && !Pawn->IsLocallyControlled())
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
    }

    float SlotNumber = TriggerEventData ? TriggerEventData->EventMagnitude : -1.f;

    APlayerCharacter* Character = Cast<APlayerCharacter>(ActorInfo->AvatarActor.Get());
    NULLCHECK_CODE_RETURN_LOG(Character, GALog, Warning, EndAbility(Handle, ActorInfo, ActivationInfo, true, false);, );
    ASC = Character->GetAbilitySystemComponent();
    NULLCHECK_CODE_RETURN_LOG(ASC, GALog, Warning, EndAbility(Handle, ActorInfo, ActivationInfo, true, false);, );
    // HeldItemComponent 찾기
    UHeldItemComponent* HeldItemComp = Character->FindComponentByClass<UHeldItemComponent>();
    NULLCHECK_CODE_RETURN_LOG(HeldItemComp, GALog, Warning, EndAbility(Handle, ActorInfo, ActivationInfo, true, false);, );

    // 선택된 아이템 타입 가져오기
    APlayerController* PlayerController = ActorInfo->PlayerController.Get();
    NULLCHECK_CODE_RETURN_LOG(PlayerController, GALog, Warning, EndAbility(Handle, ActorInfo, ActivationInfo, true, false);, );

    APS_Player* PS = Cast<APS_Player>(PlayerController->PlayerState);
    NULLCHECK_CODE_RETURN_LOG(PS, GALog, Warning, EndAbility(Handle, ActorInfo, ActivationInfo, true, false);, );

    UInventoryComponent* InventoryComponent = PS->InventoryComp;
    NULLCHECK_CODE_RETURN_LOG(InventoryComponent, GALog, Warning, EndAbility(Handle, ActorInfo, ActivationInfo, true, false);, );

    ChoiceItemType = InventoryComponent->ChoiceItem(static_cast<int32>(SlotNumber - 1));

    // ItemAbilityTable 설정 (필요한 경우)
    if (ItemAbilityTable && !HeldItemComp->ItemAbilityTable)
    {
        HeldItemComp->ItemAbilityTable = ItemAbilityTable;
    }

    // HeldItemComponent를 통해 아이템 처리
    if (ChoiceItemType == EItemType::EMP || ChoiceItemType == EItemType::NoiseBomb || ChoiceItemType == EItemType::Key)
    {
        // 투척 아이템인 경우 스폰 및 부착
        HeldItemComp->SpawnAndAttachHeldItem(ChoiceItemType);
        PlayStartHoldMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("StartHoldMontage"), StartHoldMontage, 1.0f);
        PlayStartHoldMontageTask->OnCompleted.AddDynamic(this, &UGA_HoldItem::OnMontageComplete);
        PlayStartHoldMontageTask->ReadyForActivation();

        TPT_LOG(GALog, Log, TEXT("투척 아이템 (%d) 손에 부착 완료"), static_cast<int32>(ChoiceItemType));
        // 여기서 포물선 인디케이터 출력 로직 추가 가능
    }
    else
    {
        // 투척 아이템이 아닌 경우 기존 아이템 제거
        //PlayEndHoldMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("EndHoldMontage"), EndHoldMontage, 1.0f);
        //PlayEndHoldMontageTask->OnCompleted.AddDynamic(this, &UGA_HoldItem::OnMontageComplete);
        //PlayEndHoldMontageTask->ReadyForActivation();
        HeldItemComp->DestroyHeldItem();
        TPT_LOG(GALog, Log, TEXT("투척 아이템이 아니므로 손에 있는 아이템 제거"));
        EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
    }

}

void UGA_HoldItem::OnMontageComplete()
{
	FGameplayTag InputTag = FTPTGameplayTags::Get().TPTGameplay_Character_State_AimItem;
    ASC->FindAbilitySpecFromInputID(static_cast<int32>(FTPTGameplayTags::Get().TagMap[InputTag]));
    ASC->TryActivateAbilitiesByTag(FGameplayTagContainer(InputTag));
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
