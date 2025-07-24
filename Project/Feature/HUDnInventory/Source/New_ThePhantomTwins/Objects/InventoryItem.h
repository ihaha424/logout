
#pragma once

#include "CoreMinimal.h"
#include "SzObjects/ItemObject.h"
#include "ItemData.h"
#include "InventoryItem.generated.h"

/**
 * 
 */
UCLASS()
class NEW_THEPHANTOMTWINS_API AInventoryItem : public AItemObject
{
	GENERATED_BODY()
	
public:
	AInventoryItem();

public:
	virtual void OnInteractServer_Implementation(const APawn* Interactor) override;

	EItemType GetEItemType(){return ItemType;}

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    EItemType ItemType;
};
