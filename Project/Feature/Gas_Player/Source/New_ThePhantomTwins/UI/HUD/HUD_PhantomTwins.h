// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "HUD_PhantomTwins.generated.h"

/**
 * 
 */
UCLASS()
class NEW_THEPHANTOMTWINS_API AHUD_PhantomTwins : public AHUD
{
	GENERATED_BODY()

protected:
    virtual void BeginPlay() override;

public:
/* PlayerStatus */
    // Player HP 변경
	UFUNCTION(BlueprintCallable, Category = "UI | PlayerStatus")
    void UpdateHP(int32 HP);    

    // Player Mental 변경
    UFUNCTION(BlueprintCallable, Category = "UI | PlayerStatus")
    void UpdateMental(int32 Mental);    

    // Player 초상화 세팅
    UFUNCTION(BlueprintCallable, Category = "UI | PlayerStatus")
    void SetCharPortrait(UTexture2D* PortraitTexture);    

    // Player Stamina 변경
    UFUNCTION(BlueprintCallable, Category = "UI | PlayerStatus")
    void UpdateStamina(int32 Stamina);    


/* ClearItem */ 
    // 현재 클리어 아이템 갯수 변경
    UFUNCTION(BlueprintCallable, Category = "UI | ClearItem")
    void UpdateClearItem(int32 CurrentClearItem);


/* Skill */
    UFUNCTION(BlueprintCallable, Category = "UI | Skill")
    void SetActiveSkillIcon(UTexture2D* ActiveSkillIcon);

    UFUNCTION(BlueprintCallable, Category = "UI | Skill")
    void SetPassiveSkillIcon(UTexture2D* PassiveSkillIcon);

    UFUNCTION(BlueprintCallable, Category = "UI | Skill")
    void UpdateSkillPoints(int32 SkillPointsNum);



protected:
// 조준점 UI
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<class UUserWidget> AimDotWidgetClass;

    UPROPERTY()
    TObjectPtr<class UUserWidget> AimDotWidget;

// PlayerStatus
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<class UPlayerStatusWidget> PlayerStatusWidgetClass;

    UPROPERTY()
    TObjectPtr<class UPlayerStatusWidget> PlayerStatusWidget;


    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<class UPlayerStaminaWidget> PlayerStaminaWidgetClass;

    UPROPERTY()
    TObjectPtr<class UPlayerStaminaWidget> PlayerStaminaWidget;


// ClearItem
     UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<class UClearItemCounterWidget> ClearItemCounterWidgetClass;

    UPROPERTY()
    TObjectPtr<class UClearItemCounterWidget> ClearItemCounterWidget;


// Skill
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<class UPlayerSkillWidget> PlayerSkillWidgetClass;

    UPROPERTY()
    TObjectPtr<class UPlayerSkillWidget> PlayerSkillWidget;


// Inventory
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<class UInventoryWidget> InventoryWidgetClass;

    UPROPERTY()
    TObjectPtr<class UInventoryWidget> InventoryWidget;

};
