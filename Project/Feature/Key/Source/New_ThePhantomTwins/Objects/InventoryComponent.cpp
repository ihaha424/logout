#include "InventoryComponent.h"
#include "Net/UnrealNetwork.h"
#include "Player/PlayerCharacter.h"
#include "UI/HUD/PlayerHUDWidget.h"
#include "UI/HUD/InventoryWidget.h"
#include "UI/HUD/ItemSlotWidget.h"
#include "Player/PS_Player.h"
#include "Player/PC_Player.h"
#include "GameplayCueNotify_Static.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Log/TPTLog.h"
#include "Kismet/GameplayStatics.h"
#include "Player/FocusTraceComponent.h"
#include "Objects/Door.h"


UInventoryComponent::UInventoryComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    SetIsReplicatedByDefault(true);
}

void UInventoryComponent::BeginPlay()
{
    Super::BeginPlay();
    InventorySlots.Init(FItemSlot(), MaxInventorySlots);
}

void UInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(UInventoryComponent, InventorySlots);
}

// ==============================
//          Public API
// ==============================
void UInventoryComponent::AddItem(EItemType eItemType)
{
    // 로컬에서 즉시 인벤토리 UI를 보이게 함 (클라이언트가 아이템 획득했을 때 시각적 피드백)
    VisibleInventory();

    // 클라에서 호출되면 서버로 포워딩 (서버가 실제 데이터 변경 담당)
    if (GetOwnerRole() != ROLE_Authority)
    {
        C2S_AddItem(eItemType);
        return;
    }

    // 서버에서만 실제 수정
    AddItem_ServerAuth(eItemType);
    // 호스트(서버)의 HUD도 갱신 필요 (OnRep는 서버에서 안 뜸)
    RefreshUIFromInventory();
}

EItemType UInventoryComponent::UseItem(int32 SlotIndex)
{
    // 입력은 1-based 인덱스
    if (SlotIndex <= 0 || SlotIndex > InventorySlots.Num())
    {
        return EItemType::None;
    }

    // 아이템 타입 미리 가져오기
    const FItemSlot& LocalSlot = InventorySlots[SlotIndex - 1];
    EItemType ItemType = LocalSlot.ItemType;

    // 조건 검증 - 서버와 클라이언트 모두에서 체크
    if (!CanUseItem(ItemType, SlotIndex))
    {
        // 조건을 만족하지 않으면 아무것도 하지 않음 (UI 변경도 없음)
        return EItemType::None;
    }

    VisibleInventory();

    if (GetOwnerRole() != ROLE_Authority)
    {
        // 클라: 이미 위에서 선언한 LocalSlot을 재사용
        EItemType Predicted = (LocalSlot.ItemQuantity > 0) ? LocalSlot.ItemType : EItemType::None;

        // 서버에 실제 사용 요청
        C2S_UseItem(SlotIndex);
        selectedNum = -1;
        return Predicted;
    }

    // 서버: 실제 사용 처리
    EItemType Used = UseItem_ServerAuth(SlotIndex);
    // 호스트 HUD 갱신
    RefreshUIFromInventory();
    selectedNum = -1;
    return Used;
}

EItemType UInventoryComponent::ChoiceItem(int32 SlotIndex)
{
    // 0-based 인덱스라면 범위 체크
    if (SlotIndex < 0 || SlotIndex >= InventorySlots.Num())
    {
        return EItemType::None;
    }

    if (PlayerHUDWidget)
    {
        if (IsSlotEmpty(InventorySlots[SlotIndex])) return EItemType::None;
        if (selectedNum == -1 || selectedNum != SlotIndex)
        {
            if (selectedNum != -1)
            {
                PlayerHUDWidget->SetOutline(selectedNum, false);
                PlayerHUDWidget->SetToolTips(false, InventorySlots[selectedNum].ItemType);
            }

            VisibleInventory();
            selectedNum = SlotIndex;
            PlayerHUDWidget->SetOutline(SlotIndex, true);
            PlayerHUDWidget->SetToolTips(true, InventorySlots[SlotIndex].ItemType);
            // 3초 뒤 비활성화
            FTimerHandle TimerHandle;
            GetWorld()->GetTimerManager().SetTimer(
                TimerHandle,
                FTimerDelegate::CreateWeakLambda(this, [this, SlotIndex]()
                    {
                        if (PlayerHUDWidget)
                        {
                            PlayerHUDWidget->SetOutline(SlotIndex, false);
                            PlayerHUDWidget->SetToolTips(false, InventorySlots[SlotIndex].ItemType);
                        }
                    }),
                3.0f,
                false
            );
        }
    }
    return InventorySlots[SlotIndex].ItemType;
}

bool UInventoryComponent::SetPlayerHUDWidget(class UPlayerHUDWidget* HUDWidget)
{
    PlayerHUDWidget = HUDWidget;
    NULLCHECK_RETURN_LOG(GetOwner(), PlayerLog, Error, false);
    APS_Player* PS = Cast<APS_Player>(GetOwner());
    if (!PS)
    {
        return false;
    }
    APC_Player* PC = Cast<APC_Player>(PS->GetPlayerController());
    APlayerCharacter* OwnerPlayer = Cast<APlayerCharacter>(PC ? PC->GetCharacter() : nullptr);
    UPlayerHUDWidget* TempWidget = OwnerPlayer ? OwnerPlayer->GetPlayerHUDWidget() : nullptr;
    return (PlayerHUDWidget.Get() == TempWidget);
}

void UInventoryComponent::OnRep_InventorySlots()
{
    VisibleInventory();
    // 클라에서 복제 도착 시 UI 반영
    RefreshUIFromInventory();
}

// ==============================
//          Server RPC
// ==============================
void UInventoryComponent::C2S_AddItem_Implementation(EItemType eItemType)
{
    AddItem_ServerAuth(eItemType);
    // 서버에서 배열 변경 후, 클라들은 OnRep으로 UI 갱신.
    // 호스트는 직접 갱신.
    RefreshUIFromInventory();
}

void UInventoryComponent::C2S_UseItem_Implementation(int32 SlotIndex)
{
    // 서버에서도 한번 더 검증 (보안을 위해)
    if (SlotIndex <= 0 || SlotIndex > InventorySlots.Num())
    {
        return;
    }

    const FItemSlot& LocalSlot = InventorySlots[SlotIndex - 1];
    EItemType ItemType = LocalSlot.ItemType;

    if (!CanUseItem(ItemType, SlotIndex))
    {
        // 서버에서 조건 불만족시 아무것도 하지 않음
        return;
    }

    UseItem_ServerAuth(SlotIndex);
    RefreshUIFromInventory();
}

// ==============================
//      Server-only logic
// ==============================
void UInventoryComponent::AddItem_ServerAuth(EItemType eItemType)
{
    // 같은 아이템 스택 증가 시도
    for (int32 i = 0; i < InventorySlots.Num(); ++i)
    {
        if (InventorySlots[i].ItemType == eItemType)
        {
            if (InventorySlots[i].ItemQuantity < MaxQuantity)
            {
                InventorySlots[i].ItemQuantity++;
                return;
            }
            // 스택 꽉 찼으면 계속 탐색 (다른 빈칸 찾기)
        }
    }

    // 빈 슬롯 탐색(가장 앞부터 —> 비워진 1번이 우선 채워짐)
    for (int32 i = 0; i < InventorySlots.Num(); ++i)
    {
        if (InventorySlots[i].ItemQuantity == 0 || InventorySlots[i].ItemType == EItemType::None)
        {
            InventorySlots[i].ItemType = eItemType;
            InventorySlots[i].ItemQuantity = 1;
            return;
        }
    }

    // 빈 칸 없음: 아무 것도 안 함
}

EItemType UInventoryComponent::UseItem_ServerAuth(int32 SlotIndex)
{
    if (SlotIndex <= 0 || SlotIndex > InventorySlots.Num())
    {
        return EItemType::None;
    }

    FItemSlot& itemSlot = InventorySlots[SlotIndex - 1];
    if (itemSlot.ItemQuantity <= 0 || itemSlot.ItemType == EItemType::None)
    {
        return EItemType::None;
    }

    EItemType usedItemType = itemSlot.ItemType;
    if (itemSlot.ItemQuantity > 1)
    {
        itemSlot.ItemQuantity--;
    }
    else
    {
        // 수량 1 사용 후 슬롯 비우기 (→ 다음 AddItem시 1번부터 채워짐)
        itemSlot.ItemType = EItemType::None;
        itemSlot.ItemQuantity = 0;
    }

    // 효과는 서버에서 적용 (GAS가 복제해줌)
    ExecuteItemEffects(usedItemType);
    selectedNum = -1;
    return usedItemType;
}

// ==============================
//        UI / Effects
// ==============================
void UInventoryComponent::RefreshUIFromInventory()
{
    if (!PlayerHUDWidget) return;
    for (int32 i = 0; i < InventorySlots.Num(); ++i)
    {
        const FItemSlot& Slot = InventorySlots[i];
        if (Slot.ItemQuantity > 0 && Slot.ItemType != EItemType::None)
        {
            PlayerHUDWidget->SetItemIcon(i, Slot.ItemType);
            PlayerHUDWidget->SetItemQuantity(i, Slot.ItemQuantity);
        }
        else
        {
            PlayerHUDWidget->ResetItemSlot(i);
        }
    }
}

void UInventoryComponent::VisibleInventory()
{
    if (!PlayerHUDWidget) return;
    // 기존 타이머가 있다면 제거(중복 방지)
    GetWorld()->GetTimerManager().ClearTimer(VisibleInventoryTimerHandle);
    // 인벤토리(UI) 활성화
    PlayerHUDWidget->VisibleInventory(true);
    // 5초 뒤에 인벤토리(UI) 비활성화
    GetWorld()->GetTimerManager().SetTimer(
        VisibleInventoryTimerHandle,
        FTimerDelegate::CreateWeakLambda(this, [this]()
            {
                if (PlayerHUDWidget)
                {
                    PlayerHUDWidget->VisibleInventory(false);
                }
            }),
        5.0f, // 초 단위
        false // 반복 아님
    );
}

void UInventoryComponent::ExecuteItemEffects(EItemType ItemType)
{
    APS_Player* PS = Cast<APS_Player>(GetOwner());
    APC_Player* PC = PS ? Cast<APC_Player>(PS->GetPlayerController()) : nullptr;
    if (!PC)
    {
        TPT_LOG(HUDLog, Warning, TEXT("OwnerPlayer Controller is null in ExecuteItem"));
        return;
    }
    if (ItemType == EItemType::None) return;
    FItemDataTable* ItemData = GetItemAbilityData(ItemType);
    if (!ItemData)
    {
        TPT_LOG(HUDLog, Warning, TEXT("ItemAbilityData not found for ItemType: %d"), (int32)ItemType);
        return;
    }
    if (ItemData->GameAbility)
    {
        ExecuteGameplayAbility(ItemType, ItemData->GameAbility);
    }
    if (ItemData->GameEffect)
    {
        ApplyGameplayEffect(ItemData->GameEffect);
    }
}

FItemDataTable* UInventoryComponent::GetItemAbilityData(EItemType ItemType)
{
    if (!ItemAbilityTable) return nullptr;
    FString FullEnumName = UEnum::GetValueAsString(ItemType);
    FString EnumName;
    if (!FullEnumName.Split(TEXT("::"), nullptr, &EnumName))
    {
        EnumName = FullEnumName;
    }
    return ItemAbilityTable->FindRow<FItemDataTable>(FName(*EnumName), TEXT("GetItemAbilityData"));
}

void UInventoryComponent::ExecuteGameplayAbility(EItemType ItemType, TSubclassOf<UGameplayAbility> AbilityClass)
{
    APS_Player* PS = Cast<APS_Player>(GetOwner());
    APC_Player* PC = PS ? Cast<APC_Player>(PS->GetPlayerController()) : nullptr;
    APlayerCharacter* OwnerPlayer = PC ? Cast<APlayerCharacter>(PC->GetCharacter()) : nullptr;
    UAbilitySystemComponent* OwnerASC = OwnerPlayer ? OwnerPlayer->GetAbilitySystemComponent() : nullptr;
    if (!OwnerASC) return;

    // GameplayEventData 생성
    FGameplayEventData EventData;
    EventData.EventMagnitude = static_cast<float>((int32)ItemType); // ItemType 전달
    EventData.Instigator = OwnerPlayer;
    EventData.Target = OwnerPlayer;

    // GiveAbilityAndActivateOnce - EventData와 함께 실행
    FGameplayAbilitySpec AbilitySpec(AbilityClass, 1);
    OwnerASC->GiveAbilityAndActivateOnce(AbilitySpec, &EventData);
}

void UInventoryComponent::ApplyGameplayEffect(TSubclassOf<UGameplayEffect> EffectClass)
{
    APS_Player* PS = Cast<APS_Player>(GetOwner());
    APC_Player* PC = PS ? Cast<APC_Player>(PS->GetPlayerController()) : nullptr;
    APlayerCharacter* OwnerPlayer = PC ? Cast<APlayerCharacter>(PC->GetCharacter()) : nullptr;
    UAbilitySystemComponent* OwnerASC = OwnerPlayer ? OwnerPlayer->GetAbilitySystemComponent() : nullptr;
    if (!OwnerASC) return;
    FGameplayEffectContextHandle EffectContext = OwnerASC->MakeEffectContext();
    EffectContext.AddSourceObject(OwnerPlayer);
    FGameplayEffectSpecHandle EffectSpecHandle = OwnerASC->MakeOutgoingSpec(EffectClass, 1, EffectContext);
    if (EffectSpecHandle.IsValid())
    {
        OwnerASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
    }
}


bool UInventoryComponent::CanUseItem(EItemType ItemType, int32 SlotIndex)
{
    // 기본 조건들 먼저 체크
    if (SlotIndex <= 0 || SlotIndex > InventorySlots.Num())
    {
        return false;
    }

    const FItemSlot& itemSlot = InventorySlots[SlotIndex - 1];
    if (itemSlot.ItemQuantity <= 0 || itemSlot.ItemType == EItemType::None)
    {
        return false;
    }

    switch (ItemType)
    {
    case EItemType::Key:
        return CanUseKey();

    default:
        return true;
    }
}

bool UInventoryComponent::CanUseKey()
{
    APS_Player* PS = Cast<APS_Player>(GetOwner());
    if (!PS) return false;

    APC_Player* PC = Cast<APC_Player>(PS->GetPlayerController());
    if (!PC) return false;

    APlayerCharacter* OwnerPlayer = Cast<APlayerCharacter>(PC->GetCharacter());
    if (!OwnerPlayer) return false;

    AActor* TargetActor = Cast<AActor>(OwnerPlayer->GetFocusTrace()->GetFocusedActor());

    // 사용 불가 시 위젯 띄우고 타이머 설정하는 함수
    auto ShowCannotUseWidget = [PC]()
        {
            PC->SetWidget(TEXT("CannotUseItem"), true, EMessageTargetType::LocalClient);

            FTimerHandle TimerHandle;
            FTimerDelegate TimerDel;
            TimerDel.BindLambda([PC]()
                {
                    PC->SetWidget(TEXT("CannotUseItem"), false, EMessageTargetType::LocalClient);
                });

            PC->GetWorldTimerManager().SetTimer(TimerHandle, TimerDel, 2.0f, false);
        };

    // TargetActor 검사
    if (!TargetActor || !TargetActor->ActorHasTag(TEXT("KeyInteract")))
    {
        TPT_LOG(ObjectLog, Log, TEXT("UInventoryComponent::CanUseKey() : 키 사용 불가 - 태그 없음"));
        ShowCannotUseWidget();
        return false;
    }

    // KeyInteract 태그가 있을 때, Door 사용 여부 검사
    ADoor* TargetDoor = Cast<ADoor>(TargetActor);
    if (TargetDoor && TargetDoor->bKeyUsed)
    {
        TPT_LOG(ObjectLog, Log, TEXT("UInventoryComponent::CanUseKey() : 키 사용 불가 - 이미 사용됨"));
        ShowCannotUseWidget();
        return false;
    }

    TPT_LOG(ObjectLog, Log, TEXT("UInventoryComponent::CanUseKey() : 키 사용 가능"));
    return true;
}