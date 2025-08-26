
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

void UInventoryComponent::AddItem(EItemType eItemType)
{
    int32 EmptySlotIndex = INDEX_NONE;

    for (int32 i = 0; i < InventorySlots.Num(); ++i)
    {
        // 같은 아이템을 찾음.
        if (InventorySlots[i].ItemType == eItemType)
        {
            // 같은 아이템이 있고 스택이 최대 미만이면
            if (InventorySlots[i].ItemQuantity < MaxQuantity)
            {
                // 스택 증가
                InventorySlots[i].ItemQuantity++;

                // 인벤토리 위젯 : 수량 텍스트 변경 (OnRep_InventorySlots 에서도 해줘야 함)
                if (PlayerHUDWidget)
                {
                    PlayerHUDWidget->SetItemQuantity(i, InventorySlots[i].ItemQuantity);
                }

                return;
            }
            // 스택이 가득 찬 동일 아이템이어도, 처리하지 않고 계속 탐색
        }
        else if (InventorySlots[i].ItemQuantity == 0 && EmptySlotIndex == INDEX_NONE)
        {
            // 처음 만나는 빈 칸의 인덱스 저장 (추가는 나중에)
            EmptySlotIndex = i;
        }
    }

    // 루프 끝까지 같은 아이템 못 찾음 == 지금까지 아무 것도 추가 안됨
    // 빈 칸이 있으면 빈 칸에 신규로 추가
    if (EmptySlotIndex != INDEX_NONE)
    {
        InventorySlots[EmptySlotIndex].ItemType = eItemType;
        InventorySlots[EmptySlotIndex].ItemQuantity = 1;

        // 인벤토리 위젯 변경 (OnRep_InventorySlots 에서도 해줘야 함)
        if (PlayerHUDWidget)
        {
            // 아이템 아이콘 변경
            PlayerHUDWidget->SetItemIcon(EmptySlotIndex, eItemType);

            // 수량 텍스트 변경
            PlayerHUDWidget->SetItemQuantity(EmptySlotIndex, 1);
        }


        return;
    }
    
    // 여기까지 왔으면 빈 칸도 없고, 기존 아이템도 스택 꽉 찬 상태 => 더 이상 넣을 수 없음
}

void UInventoryComponent::ChoiceItem(int32 SlotIndex)
{
// 슬롯 아웃라인 설정
    // selectedNum과 다른 번호면 아웃라인, 툴팁 활성화
    // selectedNum과 같은 번호면 아웃라인, 툴팁 비활성화 

    // 새로운 슬롯을 눌렀으면 실행
    if (PlayerHUDWidget)
    {
        if (IsSlotEmpty(InventorySlots[SlotIndex])) return;

        if (selectedNum == -1 || selectedNum != SlotIndex)
        {
            // 플레이어가 누른 번호 저장
            selectedNum = SlotIndex;

            // 바로 활성화
            PlayerHUDWidget->SetOutline(SlotIndex, true);
            PlayerHUDWidget->SetToolTips(true, InventorySlots[SlotIndex].ItemType);

            // 3초 뒤에 비활성화하도록 타이머 등록
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
                3.0f,  // 지연 시간 (초)
                false  // 반복 여부 (false = 1회 실행)
            );
        }
    }
}

EItemType UInventoryComponent::UseItem(int32 SlotIndex)
{
    //TPT_LOG(HUDLog, Log, TEXT("UInventoryComponent::UseItem : %f"), SlotIndex);

    // 예외처리 (슬롯 인덱스 제외한 숫자가 들어 온 경우.)
    if (SlotIndex <= 0 || SlotIndex > InventorySlots.Num())
    {
        return EItemType::None;
    }

    FItemSlot& itemSlot = InventorySlots[SlotIndex - 1];

    // itemSlot.ItemQuantity가 0 이면(빈 슬롯이면) : return EItemType::None;
    if (itemSlot.ItemQuantity <= 0 || itemSlot.ItemType == EItemType::None)
    {
        return EItemType::None;
    }

    // 현재 아이템 타입 반환 (사용한 아이템 종류)
    EItemType usedItemType = itemSlot.ItemType;

    // itemSlot.ItemQuantity가 2 이상이면 아이템 수량 감소 처리
    if (itemSlot.ItemQuantity > 1)
    {
        itemSlot.ItemQuantity--;

        // 인벤토리 위젯 : 수량 텍스트 변경 (OnRep_InventorySlots 에서도 해줘야 함)
		if (PlayerHUDWidget)
		{
            PlayerHUDWidget->SetItemQuantity(SlotIndex - 1, itemSlot.ItemQuantity);
        }
    }
    else
    {
        // 수량이 1이면 아이템 제거, 슬롯 초기화
        itemSlot.ItemType = EItemType::None;
        itemSlot.ItemQuantity = 0;

        // 인벤토리 위젯 : 초기화
        if (PlayerHUDWidget)
        {
            PlayerHUDWidget->ResetItemSlot(SlotIndex - 1);
        }
    }

    // 아이템 효과 발동
    ExecuteItemEffects(usedItemType);

    selectedNum = -1;

    return usedItemType;
}

bool UInventoryComponent::SetPlayerHUDWidget(class UPlayerHUDWidget* HUDWidget)
{
    // 인자로 받은 위젯을 먼저 저장
    PlayerHUDWidget = HUDWidget;

    // OwnerPlayer에서 현재 위젯을 다시 받아옴
    
    NULLCHECK_RETURN_LOG(GetOwner(),PlayerLog,Error, false);

    APS_Player* PS = Cast<APS_Player>(GetOwner());

	if (!PS)
	{
		return false; // Owner가 없으면 false
	}

   APC_Player* PC = Cast<APC_Player>(PS->GetPlayerController());
   APlayerCharacter* OwnerPlayer = Cast<APlayerCharacter>(PC->GetCharacter());
   UPlayerHUDWidget* TempWidget = OwnerPlayer->GetPlayerHUDWidget();

    // 두 포인터가 같은 객체를 가리키는지 비교해서 반환
   return (PlayerHUDWidget.Get() == TempWidget);
}

void UInventoryComponent::OnRep_InventorySlots()
{
    if (!PlayerHUDWidget)
        return;

    for (int32 i = 0; i < InventorySlots.Num(); ++i)
    {
        const FItemSlot& Slot = InventorySlots[i];

        if (Slot.ItemQuantity > 0 && Slot.ItemType != EItemType::None)
        {
            // 아이템 정보 갱신
            PlayerHUDWidget->SetItemIcon(i, Slot.ItemType);
            PlayerHUDWidget->SetItemQuantity(i, Slot.ItemQuantity);
        }
        else
        {
            // 아이템이 없으니 슬롯 리셋
            PlayerHUDWidget->ResetItemSlot(i);
        }
    }
}

void UInventoryComponent::ExecuteItemEffects(EItemType ItemType)
{
    APS_Player* PS = Cast<APS_Player>(GetOwner());
    APC_Player* PC = Cast<APC_Player>(PS->GetPlayerController());

    if (!PC)
    {
        TPT_LOG(HUDLog, Warning, TEXT("OwnerPlayer Controller is null in ExecuteItem"));
        return;
    }

    // 아이템 유효성 검사
    if (ItemType == EItemType::None) return;


    // DataTable에서 아이템 데이터 가져오기
    FItemDataTable* ItemData = GetItemAbilityData(ItemType);

    if (!ItemData)
    {
        TPT_LOG(HUDLog, Warning, TEXT("ItemAbilityData not found for ItemType: %d"), (int32)ItemType);
        return;
    }

    // GameplayAbility 실행
    if (ItemData->GameAbility)
    {
        ExecuteGameplayAbility(ItemData->GameAbility);
    }

    // GameplayEffect 적용
    if (ItemData->GameEffect)
    {
        ApplyGameplayEffect(ItemData->GameEffect);
    }

}


FItemDataTable* UInventoryComponent::GetItemAbilityData(EItemType ItemType)
{
    if(!ItemAbilityTable) return nullptr;

    // EItemType을 문자열로 변환 (예: "EItemType::FireCracker")
    FString FullEnumName = UEnum::GetValueAsString(ItemType);

    // "::" 기준으로 분리해서 enum 이름만 남기기
    FString EnumName;
    if (!FullEnumName.Split(TEXT("::"), nullptr, &EnumName))
    {
        // Split 실패 시 바로 전체 이용
        EnumName = FullEnumName;
    }

    // 디버그 로그 (실제 RowName과 일치하는지 꼭 확인)
    //TPT_LOG(HUDLog, Log, TEXT("Looking for RowName: %s"), *EnumName);

    // DataTable에서 검색
    return ItemAbilityTable->FindRow<FItemDataTable>(FName(*EnumName), TEXT("GetItemAbilityData"));
}

void UInventoryComponent::ExecuteGameplayAbility(TSubclassOf<UGameplayAbility> AbilityClass)
{
    APS_Player* PS = Cast<APS_Player>(GetOwner());
    APC_Player* PC = Cast<APC_Player>(PS->GetPlayerController());

    APlayerCharacter* OwnerPlayer = Cast<APlayerCharacter>(PC->GetCharacter());
    UAbilitySystemComponent* OwnerASC = OwnerPlayer->GetAbilitySystemComponent();

    // 임시로 Ability를 부여하고 즉시 실행
    FGameplayAbilitySpec AbilitySpec(AbilityClass, 1);
    FGameplayAbilitySpecHandle Handle = OwnerASC->GiveAbility(AbilitySpec);

    if (Handle.IsValid())
    {
        OwnerASC->TryActivateAbility(Handle);
        // 사용 후 제거 (일회성 아이템의 경우)
        OwnerASC->ClearAbility(Handle);
    }
}

void UInventoryComponent::ApplyGameplayEffect(TSubclassOf<UGameplayEffect> EffectClass)
{
    APS_Player* PS = Cast<APS_Player>(GetOwner());
    APC_Player* PC = Cast<APC_Player>(PS->GetPlayerController());

    APlayerCharacter* OwnerPlayer = Cast<APlayerCharacter>(PC->GetCharacter());
    UAbilitySystemComponent* OwnerASC = OwnerPlayer->GetAbilitySystemComponent();

    // GameplayEffect 적용
    FGameplayEffectContextHandle EffectContext = OwnerASC->MakeEffectContext();
    EffectContext.AddSourceObject(OwnerPlayer);

    FGameplayEffectSpecHandle EffectSpecHandle = OwnerASC->MakeOutgoingSpec(EffectClass, 1, EffectContext);
    if (EffectSpecHandle.IsValid())
    {
        OwnerASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
    }
}
