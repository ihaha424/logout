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

    //UPROPERTY(EditAnywhere, BlueprintReadWrite)
    //TObjectPtr<class UTexture2D> ItemIcon;
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class NEW_THEPHANTOMTWINS_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInventoryComponent();

protected:
	virtual void BeginPlay() override;

protected:
    UPROPERTY(EditDefaultsOnly)
    TObjectPtr<UDataTable> ItemAbilityTable;

    TArray<FItemSlot> InventorySlots;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    int32 MaxInventorySlots = 5;    // InventorySlots의 원소 수.

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    int32 MaxQuantity = 5;  // 아이템 당 최대 스택 수

public:
    void AddItem(EItemType eItemType);

    //void UseItem(int32 SlotIndex);
		
};
