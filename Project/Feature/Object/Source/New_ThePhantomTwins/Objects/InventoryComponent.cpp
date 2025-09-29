#include "InventoryComponent.h"
#include "Net/UnrealNetwork.h"
#include "Player/PlayerCharacter.h"
#include "Player/PS_Player.h"
#include "Player/PC_Player.h"
#include "Objects/Door.h"
#include "UI/HUD/PlayerHUDWidget.h"
#include "UI/QuestionBoxTextWidget.h"
#include "GameplayCueNotify_Static.h"
#include "AbilitySystemComponent.h"
#include "Log/TPTLog.h"
#include "Kismet/GameplayStatics.h"
#include "Player/FocusTraceComponent.h"

UInventoryComponent::UInventoryComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    SetIsReplicatedByDefault(true);
}

void UInventoryComponent::BeginPlay()
{
    Super::BeginPlay();

    InventorySlots.Init(FItemSlot(), MaxInventorySlots);

    if (QuestionBoxTextWidgetclass)
    {
        APS_Player* PS = Cast<APS_Player>(GetOwner());
        if (!PS) return;
        APC_Player* PC = Cast<APC_Player>(PS->GetPlayerController());
        if (PC && PC->IsLocalPlayerController())
        {
            QuestionBoxTextWidget = CreateWidget<UQuestionBoxTextWidget>(PC, QuestionBoxTextWidgetclass);
            QuestionBoxTextWidget->AddToViewport();
            QuestionBoxTextWidget->SetVisibility(ESlateVisibility::Hidden);
        }
    }
}

void UInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(UInventoryComponent, InventorySlots);
    DOREPLIFETIME(UInventoryComponent, bQuestionBoxWidgetActived);
    DOREPLIFETIME(UInventoryComponent, QuestionBoxText);
}

// Public API

void UInventoryComponent::AddItem(EItemType eItemType)
{
    VisibleInventory();

    if (GetOwnerRole() != ROLE_Authority)
    {
        C2S_AddItem(eItemType);
        return;
    }

    AddItem_ServerAuth(eItemType);
    RefreshUIFromInventory();
}

EItemType UInventoryComponent::UseItem(int32 SlotIndex)
{
    if (SlotIndex <= 0 || SlotIndex > InventorySlots.Num())
    {
        return EItemType::None;
    }

    const FItemSlot& LocalSlot = InventorySlots[SlotIndex - 1];
    EItemType ItemType = LocalSlot.ItemType;

    if (!CanUseItem(ItemType, SlotIndex))
    {
        return EItemType::None;
    }

    VisibleInventory();

    // --- HUD에서 outline/tooltips 를 즉시 제거 (클라이언트 쪽 UI)
    // SlotIndex는 1-based 이므로 0-based 인덱스로 변환
    int32 HUDIndex = FMath::Clamp(SlotIndex - 1, 0, InventorySlots.Num() - 1);
    if (PlayerHUDWidget)
    {
        PlayerHUDWidget->SetOutline(HUDIndex, false);
        PlayerHUDWidget->SetToolTips(false, ItemType);
    }
    // --------------------------------------------------------

    if (GetOwnerRole() != ROLE_Authority)
    {
        EItemType Predicted = (LocalSlot.ItemQuantity > 0) ? LocalSlot.ItemType : EItemType::None;
        C2S_UseItem(SlotIndex);
        selectedNum = -1;
        return Predicted;
    }

    EItemType Used = UseItem_ServerAuth(SlotIndex);

    // 서버(또는 호스트)인 경우에도 로컬 HUD를 확실히 갱신
    RefreshUIFromInventory();

    // (다시 안전하게 한 번 더 제거 — 위에서 이미 했지만 호스트의 경우 갱신 이후 상태가 바뀔 수 있으므로)
    if (PlayerHUDWidget)
    {
        PlayerHUDWidget->SetOutline(HUDIndex, false);
        PlayerHUDWidget->SetToolTips(false, ItemType);
    }

    selectedNum = -1;
    return Used;
}

EItemType UInventoryComponent::ChoiceItem(int32 SlotIndex)
{
    if (SlotIndex < 0 || SlotIndex >= InventorySlots.Num())
    {
        return EItemType::None;
    }

    auto ClearSelection = [this](int32 Index)
        {
            if (Index != -1 && PlayerHUDWidget)
            {
                PlayerHUDWidget->SetOutline(Index, false);
                PlayerHUDWidget->SetToolTips(false, InventorySlots[Index].ItemType);
            }
        };

    auto HighlightSlot = [this](int32 Index, bool ShowToolTip)
        {
            if (!PlayerHUDWidget) return;

            PlayerHUDWidget->SetOutline(Index, true);
            if (ShowToolTip)
            {
                PlayerHUDWidget->SetToolTips(true, InventorySlots[Index].ItemType);
            }

            GetWorld()->GetTimerManager().SetTimer(
                ChoiceItemTimerHandle,
                FTimerDelegate::CreateWeakLambda(this, [this, Index]()
                    {
                        if (PlayerHUDWidget)
                        {
                            PlayerHUDWidget->SetOutline(Index, false);
                            PlayerHUDWidget->SetToolTips(false, InventorySlots[Index].ItemType);
                        }
                    }),
                3.0f,
                false
            );
        };

    // 빈 슬롯 눌렀을 때
    if (IsSlotEmpty(InventorySlots[SlotIndex]))
    {
        ClearSelection(selectedNum);
        selectedNum = SlotIndex;
        HighlightSlot(SlotIndex, false);
        return EItemType::None;
    }

    // 아이템 있는 슬롯 눌렀을 때
    if (selectedNum == -1 || selectedNum != SlotIndex)
    {
        ClearSelection(selectedNum);
        VisibleInventory();
        selectedNum = SlotIndex;
        HighlightSlot(SlotIndex, true);
    }

    return InventorySlots[SlotIndex].ItemType;
}

bool UInventoryComponent::SetPlayerHUDWidget(UPlayerHUDWidget* HUDWidget)
{
    PlayerHUDWidget = HUDWidget;
    NULLCHECK_RETURN_LOG(GetOwner(), PlayerLog, Error, false);

    APS_Player* PS = Cast<APS_Player>(GetOwner());
    if (!PS) return false;

    APC_Player* PC = Cast<APC_Player>(PS->GetPlayerController());
    APlayerCharacter* OwnerPlayer = Cast<APlayerCharacter>(PC ? PC->GetCharacter() : nullptr);
    UPlayerHUDWidget* TempWidget = OwnerPlayer ? OwnerPlayer->GetPlayerHUDWidget() : nullptr;

    return (PlayerHUDWidget.Get() == TempWidget);
}

void UInventoryComponent::OnRep_InventorySlots()
{
    VisibleInventory();
    RefreshUIFromInventory();
}

// Server RPC Implementations

void UInventoryComponent::C2S_AddItem_Implementation(EItemType eItemType)
{
    AddItem_ServerAuth(eItemType);
    RefreshUIFromInventory();
}

void UInventoryComponent::C2S_UseItem_Implementation(int32 SlotIndex)
{
    if (SlotIndex <= 0 || SlotIndex > InventorySlots.Num())
    {
        return;
    }

    const FItemSlot& LocalSlot = InventorySlots[SlotIndex - 1];
    EItemType ItemType = LocalSlot.ItemType;

    if (!CanUseItem(ItemType, SlotIndex))
    {
        return;
    }

    UseItem_ServerAuth(SlotIndex);
    RefreshUIFromInventory();
}

// Server-side logic

void UInventoryComponent::AddItem_ServerAuth(EItemType eItemType)
{
    for (int32 i = 0; i < InventorySlots.Num(); ++i)
    {
        if (InventorySlots[i].ItemType == eItemType)
        {
            if (InventorySlots[i].ItemQuantity < GetMaxQuantity(eItemType))
            {
                InventorySlots[i].ItemQuantity++;
                return;
            }
        }
    }
    for (int32 i = 0; i < InventorySlots.Num(); ++i)
    {
        if (InventorySlots[i].ItemQuantity == 0 || InventorySlots[i].ItemType == EItemType::None)
        {
            InventorySlots[i].ItemType = eItemType;
            InventorySlots[i].ItemQuantity = 1;
            return;
        }
    }
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
        itemSlot.ItemType = EItemType::None;
        itemSlot.ItemQuantity = 0;
    }

    ExecuteItemEffects(usedItemType);
    selectedNum = -1;
    return usedItemType;
}

// UI and Effects

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

    GetWorld()->GetTimerManager().ClearTimer(VisibleInventoryTimerHandle);

    PlayerHUDWidget->VisibleInventory(true);

    GetWorld()->GetTimerManager().SetTimer(
        VisibleInventoryTimerHandle,
        FTimerDelegate::CreateWeakLambda(this, [this]()
            {
                if (PlayerHUDWidget)
                {
                    PlayerHUDWidget->VisibleInventory(false);
                }
            }),
        5.0f,
        false
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

void UInventoryComponent::SetTextQuestionBoxWidget(const FText& Text)
{
    if (QuestionBoxTextWidget)
    {
        QuestionBoxTextWidget->SetText(Text);
    }
}

void UInventoryComponent::ShowQuestionBoxWidget(bool bVisible)
{
    if (!QuestionBoxTextWidget) return;

    QuestionBoxTextWidget->SetVisibility(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
}

void UInventoryComponent::ShowQuestionBoxResult(const FText& Text, float Duration)
{
    QuestionBoxText = Text;
    bQuestionBoxWidgetActived = true;
    SetTextQuestionBoxWidget(Text);
    ShowQuestionBoxWidget(true);

    GetWorld()->GetTimerManager().ClearTimer(VisibleInventoryTimerHandle);

    GetWorld()->GetTimerManager().SetTimer(
        VisibleInventoryTimerHandle,
        FTimerDelegate::CreateWeakLambda(this, [this]()
            {
                bQuestionBoxWidgetActived = false;
                ShowQuestionBoxWidget(false);
            }),
        Duration,
        false
    );
}

void UInventoryComponent::OnRep_QuestionBoxWidgetActived()
{
    SetTextQuestionBoxWidget(QuestionBoxText);
    ShowQuestionBoxWidget(bQuestionBoxWidgetActived);
}

bool UInventoryComponent::CanAddToInventory(EItemType eItemType)
{
    for (const FItemSlot& Slot : InventorySlots)
    {
        // 슬롯이 비어있는가? 비어있으면 return true;
        if (IsSlotEmpty(Slot))
        {
            return true;
        }

        // 비어있지 않다면 eItemType과 같은 타입인가?
        if (Slot.ItemType != eItemType)
        {
            // 다른 타입이라면 넘김
            continue;
        }

        // 같은 타입이라면 eItemType의 최대 수량과 비교
        if (Slot.ItemQuantity < GetMaxQuantity(eItemType))
        {
            return true;
        }
        // Slot.ItemQuantity >= GetMaxQuantity(eItemType)인 경우 continue로 다음 슬롯 체크
    }

    // InventorySlots을 다 돌았는데도 들어갈 곳이 없다면 return false
    return false;
}

// Helper Functions

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

    FGameplayEventData EventData;
    EventData.EventMagnitude = static_cast<float>((int32)ItemType);
    EventData.Instigator = OwnerPlayer;
    EventData.Target = OwnerPlayer;

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

    APlayerCharacter* OwnerPlayer = Cast<APlayerCharacter>(PC ? PC->GetCharacter() : nullptr);
    if (!OwnerPlayer) return false;

    AActor* TargetActor = Cast<AActor>(OwnerPlayer->GetFocusTrace()->GetFocusedActor());

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

    if (!TargetActor || !TargetActor->ActorHasTag(TEXT("KeyInteract")))
    {
        TPT_LOG(ObjectLog, Log, TEXT("UInventoryComponent::CanUseKey() : 키 사용 불가 - 태그 없음"));
        ShowCannotUseWidget();
        return false;
    }

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

int32 UInventoryComponent::GetMaxQuantity(EItemType ItemType)
{
    if (!ItemAbilityTable) return -1;

    FItemDataTable* ItemData = GetItemAbilityData(ItemType);
    if (!ItemData) return -1;

    return ItemData->MaxStack;
}
