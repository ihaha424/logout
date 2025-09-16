// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "Engine/DataTable.h"
#include "ItemData.h"
#include "InventoryComponent.generated.h"

// 인벤토리 슬롯 구조체
USTRUCT(BlueprintType)
struct FItemSlot
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EItemType ItemType = EItemType::None;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 ItemQuantity = 0;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class NEW_THEPHANTOMTWINS_API UInventoryComponent : public UActorComponent
{
    GENERATED_BODY()
public:	
    UInventoryComponent();

protected:
    virtual void BeginPlay() override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
    // APlayerCharacter에서 그대로 호출해도 되도록 유지
    UFUNCTION(BlueprintCallable, Category="Inventory")
    void AddItem(EItemType eItemType);

    UFUNCTION()
    EItemType ChoiceItem(int32 SlotIndex);

    // 플레이어에서 1~MaxInventorySlots 숫자 키를 누르면 호출되는 함수. 슬롯에 있는 아이템의 EItemType이 반환됨
    UFUNCTION(BlueprintCallable, Category = "Inventory") 
    EItemType UseItem(int32 SlotIndex);

    // HUD 위젯 등록
    UFUNCTION()
    bool SetPlayerHUDWidget(class UPlayerHUDWidget* HUDWidget);

    UFUNCTION()
    void OnRep_InventorySlots();

    // 아이템 효과 실행 함수
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void ExecuteItemEffects(EItemType ItemType);

private:
    // === 서버 RPC ===
    UFUNCTION(Server, Reliable)
    void C2S_AddItem(EItemType eItemType);
    void C2S_AddItem_Implementation(EItemType eItemType);

    UFUNCTION(Server, Reliable)
    void C2S_UseItem(int32 SlotIndex);
    void C2S_UseItem_Implementation(int32 SlotIndex);

    // === 서버 전용 실제 로직 ===
    void AddItem_ServerAuth(EItemType eItemType);
    EItemType UseItem_ServerAuth(int32 SlotIndex);

    // DataTable에서 아이템 데이터 가져오기
    FItemDataTable* GetItemAbilityData(EItemType ItemType);

    // GameplayAbility 실행
    void ExecuteGameplayAbility(EItemType ItemType, TSubclassOf<UGameplayAbility> AbilityClass);

    // GameplayEffect 적용
    void ApplyGameplayEffect(TSubclassOf<UGameplayEffect> EffectClass);

    bool IsSlotEmpty(const FItemSlot& Slot)
    {
        return (Slot.ItemType == EItemType::None || Slot.ItemQuantity <= 0);
    }

    // 서버/클라 공통 UI 리프레시 (서버는 직접 호출, 클라는 OnRep에서 호출)
    void RefreshUIFromInventory();

    // 5초 뒤 인벤토리 위젯이 사라지는 함수
    void VisibleInventory();


    // 각 아이템 타입별 조건 검증 함수들
    UFUNCTION()
    bool CanUseItem(EItemType ItemType, int32 SlotIndex);

    bool CanUseKey();

    int32 GetMaxQuantity(EItemType ItemType);

protected:
    UPROPERTY(EditDefaultsOnly, ReplicatedUsing = OnRep_InventorySlots)
    TArray<FItemSlot> InventorySlots;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    TObjectPtr<UDataTable> ItemAbilityTable;

    //UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    //int32 MaxQuantity = 3;  // 아이템 당 최대 스택 수

    UPROPERTY()
    TObjectPtr<class UPlayerHUDWidget> PlayerHUDWidget;

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    int32 MaxInventorySlots = 5;    // InventorySlots의 원소 수.

private:
    UPROPERTY()
    int32 selectedNum = -1;

    UPROPERTY()
    FTimerHandle VisibleInventoryTimerHandle;
};
