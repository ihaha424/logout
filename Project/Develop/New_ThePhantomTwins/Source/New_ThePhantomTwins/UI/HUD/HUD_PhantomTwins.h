// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "GameplayEffectTypes.h"
#include "Attribute/PlayerAttributeSet.h"
#include "Player/PC_Player.h"
#include "Player/PlayerCharacter.h"
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

    void TryInitHUD();
	void ScheduleNextInit();
	void InitHUD_Internal(APC_Player* PC, APlayerCharacter* Pawn, APlayerState* PS, UAbilitySystemComponent* ASC,
	                      const UPlayerAttributeSet* AttSet);
	/* PlayerStatus */
    // Player HP 변경
    void UpdateHP(const int32 val);

    // Player Mental 변경
    void UpdateMental(const int32 val);

    // Player 초상화 세팅
    UFUNCTION(BlueprintCallable, Category = "UI | PlayerStatus")
    void SetCharPortrait(UTexture2D* PortraitTexture);    

    // Player Stamina 변경
    void UpdateStamina(const int32 val);


/* ClearItem */ 
    // 현재 클리어 아이템 갯수 변경
    UFUNCTION(BlueprintCallable, Category = "UI | ClearItem")
    void UpdateClearItem(int32 CurrentClearItem);


/* Skill */
    UFUNCTION(BlueprintCallable, Category = "UI | Skill")
    void SetActiveSkillIcon(UTexture2D* ActiveSkillIcon);

    UFUNCTION(BlueprintCallable, Category = "UI | Skill")
    void SetPassiveSkillIcon(UTexture2D* PassiveSkillIcon);

    void UpdateCoreEnergy(const  int32 val);


    bool bIsHUDInitDone = false;
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
