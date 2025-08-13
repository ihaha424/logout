// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../../Objects/ItemData.h"
#include "PlayerHUDWidget.generated.h"

/**
 * 
 */
UCLASS()
class NEW_THEPHANTOMTWINS_API UPlayerHUDWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
    virtual void NativeConstruct() override;

public:
    void InitializeWidgets(int32 HP=200, int32 Mental=100, int32 Stamina=100, int32 CoreEnergyNum=5, int32 MaxInventorySlots = 5,
                            UTexture2D* PortraitTexture=nullptr, UTexture2D* ActiveSkillIcon = nullptr, UTexture2D* PassiveSkillIcon = nullptr);

    /* PlayerStatus */
    // Player HP 변경
	UFUNCTION(BlueprintCallable, Category = "UI | PlayerStatus")
    void UpdateHP(const int32 HP);

    // Player Mental 변경
    UFUNCTION(BlueprintCallable, Category = "UI | PlayerStatus")
    void UpdateMental(const int32 Mental);

    // Player 초상화 세팅
    UFUNCTION(BlueprintCallable, Category = "UI | PlayerStatus")
    void SetCharPortrait(UTexture2D* PortraitTexture);    

    // Player Stamina 변경
    UFUNCTION(BlueprintCallable, Category = "UI | PlayerStatus")
    void UpdateStamina(const int32 Stamina);


/* ClearItem */ 
    // 현재 클리어 아이템 갯수 변경
    UFUNCTION(BlueprintCallable, Category = "UI | ClearItem")
    void UpdateClearItem(const int32 CurrentClearItem);


/* Skill */
    UFUNCTION(BlueprintCallable, Category = "UI | Skill")
    void SetActiveSkillIcon(UTexture2D* ActiveSkillIcon);

    UFUNCTION(BlueprintCallable, Category = "UI | Skill")
    void SetPassiveSkillIcon(UTexture2D* PassiveSkillIcon);

    UFUNCTION(BlueprintCallable, Category = "UI | Skill")
    void UpdateCoreEnergy(const int32 CoreEnergyNum);


/* Inventory */
    // 인벤토리 최대 슬롯 갯수
    UFUNCTION(BlueprintCallable, Category = "UI | Inventory")
    void SetMaxInventorySlots(const int32 MaxInventorySlots);

    // 아이템 아이콘 변경
    UFUNCTION(BlueprintCallable, Category = "UI | Inventory")
    void SetItemIcon(int32 SlotIndex, EItemType eItemType);

    // 아이템 수량 설정
    UFUNCTION(BlueprintCallable, Category = "UI | Inventory")
    void SetItemQuantity(int32 SlotIndex, int32 ItemStack);

    // 슬롯 초기화 - 아이콘과 수량 모두 비움
    UFUNCTION(BlueprintCallable, Category = "UI | Inventory")
    void ResetItemSlot(int32 SlotIndex);

    // 아이템 아웃라인 Visible
    UFUNCTION(BlueprintCallable, Category = "UI | Inventory")
    void SetOutline(int32 SlotIndex, bool bVisible);

    UFUNCTION(BlueprintCallable, Category = "UI | Inventory")
    void SetToolTips(bool bVisible, EItemType eItemType);


protected:
// 조준점 UI
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<class UUserWidget> AimDotWidget;

// PlayerStatus
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<class UPlayerStatusWidget> PlayerStatusWidget;

// Stamina
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<class UPlayerStaminaWidget> PlayerStaminaWidget;

// ClearItem
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<class UClearItemCounterWidget> ClearItemCounterWidget;

// Skill
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<class UPlayerSkillWidget> PlayerSkillWidget;

// Inventory
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<class UInventoryWidget> InventoryWidget;

public:
    int32 ClearItemCount = 0;       // 임시!!!!!!!!!!!! GameState에서 해줘야함!!!!!!!!!!!!!!!

};
