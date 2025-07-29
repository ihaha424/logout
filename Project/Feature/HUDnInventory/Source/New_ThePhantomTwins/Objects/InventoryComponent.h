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
    UFUNCTION()
    void AddItem(EItemType eItemType);

    // 플레이어에서 1~MaxInventorySlots 숫자 키를 누르면 호출되는 함수. 슬롯에 있는 아이템의 EItemType이 반환됨
    UFUNCTION(BlueprintCallable, Category = "Inventory") 
    EItemType UseItem(int32 SlotIndex);

    // HUD 위젯을 InventoryComponent에 등록
    UFUNCTION()
    bool SetPlayerHUDWidget(class UPlayerHUDWidget* HUDWidget);
    
    UFUNCTION()
    void OnRep_InventorySlots();

protected:
    UPROPERTY(EditDefaultsOnly, ReplicatedUsing = OnRep_InventorySlots)
    TArray<FItemSlot> InventorySlots;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    TObjectPtr<UDataTable> ItemAbilityTable;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    int32 MaxQuantity = 5;  // 아이템 당 최대 스택 수
    UPROPERTY()
    TObjectPtr<class UPlayerHUDWidget> PlayerHUDWidget;  // InventoryWidget 달아주고 로직 추가해야 함

    UPROPERTY()
    TObjectPtr<class APlayerCharacter> OwnerPlayer;

    UPROPERTY()
    TObjectPtr<class UAbilitySystemComponent> OwnerASC;

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    int32 MaxInventorySlots = 5;    // InventorySlots의 원소 수.


public:
    // 아이템 효과 실행 함수
    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void ExecuteItemEffects(EItemType ItemType);

private:
    // DataTable에서 아이템 데이터 가져오기
    FItemDataTable* GetItemAbilityData(EItemType ItemType);

    // GameplayAbility 실행
    void ExecuteGameplayAbility(TSubclassOf<UGameplayAbility> AbilityClass);

    // GameplayEffect 적용
    void ApplyGameplayEffect(TSubclassOf<UGameplayEffect> EffectClass);
};
