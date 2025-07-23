// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "InventoryComponent.generated.h"

// 인벤토리 슬롯 구조체
//USTRUCT(BlueprintType)
//struct FInventorySlot
//{
//    GENERATED_BODY()
//
//    UPROPERTY(EditAnywhere, BlueprintReadWrite)
//    TSubclassOf<class AItemObject> ItemClass;
//
//    UPROPERTY(EditAnywhere, BlueprintReadWrite)
//    int32 StackCount = 0;
//
//    UPROPERTY(EditAnywhere, BlueprintReadWrite)
//    int32 MaxStackSize = 1;
//
//    UPROPERTY(EditAnywhere, BlueprintReadWrite)
//    FGameplayTag ItemTag;
//
//    FInventorySlot()
//    {
//        ItemClass = nullptr;
//        StackCount = 0;
//        MaxStackSize = 1;
//    }
//
//    bool IsEmpty() const { return ItemClass == nullptr || StackCount <= 0; }
//    bool CanStack(TSubclassOf<AItemObject> OtherItemClass, int32 Amount) const;
//};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class NEW_THEPHANTOMTWINS_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInventoryComponent();

protected:
	virtual void BeginPlay() override;

//protected:
//// 5개 슬롯 인벤토리 배열
//    UPROPERTY(ReplicatedUsing = OnRep_InventorySlots, BlueprintReadOnly)
//    TArray<FInventorySlot> InventorySlots;
//
//    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
//    int32 MaxInventorySlots = 5;
//
//public:
//    // 서버 함수들
//    UFUNCTION(Server, Reliable, BlueprintCallable)
//    void Server_AddItem(TSubclassOf<AItemObject> ItemClass, int32 Amount = 1);
//
//    UFUNCTION(Server, Reliable, BlueprintCallable)
//    void Server_UseItem(int32 SlotIndex);
//
//    UFUNCTION(Server, Reliable, BlueprintCallable)
//    void Server_RemoveItem(int32 SlotIndex, int32 Amount = 1);
//
//    // 클라이언트 업데이트
//    UFUNCTION()
//    void OnRep_InventorySlots();
//
//    // 유틸리티 함수들
//    UFUNCTION(BlueprintCallable)
//    bool HasRoom(TSubclassOf<AItemObject> ItemClass, int32 Amount) const;
//
//    UFUNCTION(BlueprintCallable)
//    int32 FindItemSlot(TSubclassOf<AItemObject> ItemClass) const;
//
//    // UI 업데이트 델리게이트
//    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInventoryUpdated, int32, SlotIndex);
//    UPROPERTY(BlueprintAssignable)
//    FOnInventoryUpdated OnInventoryUpdated;
		
};
