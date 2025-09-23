// InventoryComponent.h
#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "Engine/DataTable.h"
#include "ItemData.h"
#include "InventoryComponent.generated.h"

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

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class NEW_THEPHANTOMTWINS_API UInventoryComponent : public UActorComponent
{
    GENERATED_BODY()

public:	
    UInventoryComponent();

protected:
    virtual void BeginPlay() override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
    // Public API
    UFUNCTION(BlueprintCallable, Category="Inventory")
    void AddItem(EItemType eItemType);

    UFUNCTION(BlueprintCallable, Category="Inventory")
    EItemType UseItem(int32 SlotIndex);

    UFUNCTION()
    EItemType ChoiceItem(int32 SlotIndex);

    UFUNCTION()
    bool SetPlayerHUDWidget(class UPlayerHUDWidget* HUDWidget);

    UFUNCTION()
    void OnRep_InventorySlots();

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void ExecuteItemEffects(EItemType ItemType);

    UFUNCTION(BlueprintCallable, Category="Inventory | QuestionBox")
    void ShowQuestionBoxResult(const FText& Text, float Duration);

    UFUNCTION()
    void SetTextQuestionBoxWidget(const FText& Text);

    UFUNCTION()
    void ShowQuestionBoxWidget(bool bVisible);

    UFUNCTION()
    void OnRep_QuestionBoxWidgetActived();

    // 인벤토리에 넣을 수 있는지 확인
    UFUNCTION()
    bool CanAddToInventory(EItemType eItemType);


public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    int32 MaxInventorySlots = 5;

    UPROPERTY(EditDefaultsOnly, ReplicatedUsing=OnRep_QuestionBoxWidgetActived)
    bool bQuestionBoxWidgetActived = false;

    UPROPERTY(EditDefaultsOnly, Replicated)
    FText QuestionBoxText;

protected:
    UPROPERTY(EditDefaultsOnly, ReplicatedUsing=OnRep_InventorySlots)
    TArray<FItemSlot> InventorySlots;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    TObjectPtr<UDataTable> ItemAbilityTable;

    UPROPERTY()
    TObjectPtr<class UPlayerHUDWidget> PlayerHUDWidget;

    UPROPERTY(EditDefaultsOnly, Category = "Inventory | QuestionBox")
    TSubclassOf<class UQuestionBoxTextWidget> QuestionBoxTextWidgetclass;

    UPROPERTY()
    TObjectPtr<class UQuestionBoxTextWidget> QuestionBoxTextWidget;

private:
    int32 selectedNum = -1;

    FTimerHandle VisibleInventoryTimerHandle;

    // Server RPCs
    UFUNCTION(Server, Reliable)
    void C2S_AddItem(EItemType eItemType);
    void C2S_AddItem_Implementation(EItemType eItemType);

    UFUNCTION(Server, Reliable)
    void C2S_UseItem(int32 SlotIndex);
    void C2S_UseItem_Implementation(int32 SlotIndex);

    // Server-only methods
    void AddItem_ServerAuth(EItemType eItemType);
    EItemType UseItem_ServerAuth(int32 SlotIndex);

    // Helpers
    FItemDataTable* GetItemAbilityData(EItemType ItemType);
    void ExecuteGameplayAbility(EItemType ItemType, TSubclassOf<UGameplayAbility> AbilityClass);
    void ApplyGameplayEffect(TSubclassOf<UGameplayEffect> EffectClass);

    bool IsSlotEmpty(const FItemSlot& Slot) const
    {
        return (Slot.ItemType == EItemType::None || Slot.ItemQuantity <= 0);
    }

    void RefreshUIFromInventory();
    void VisibleInventory();

    UFUNCTION()
    bool CanUseItem(EItemType ItemType, int32 SlotIndex);

    bool CanUseKey();
    int32 GetMaxQuantity(EItemType ItemType);
};
