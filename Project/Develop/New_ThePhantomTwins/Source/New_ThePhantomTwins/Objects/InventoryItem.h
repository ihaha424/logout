
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

protected:
	virtual void BeginPlay() override;

public:
	virtual bool CanInteract_Implementation(const APawn* Interactor, bool bIsDetected) override;

	void SetWidgetVisible(const APawn* Interactor, bool bVisible, EItemType eItemType);

	virtual void OnInteractServer_Implementation(const APawn* Interactor) override;

	EItemType GetEItemType(){return ItemType;}

	// 블루프린트에서 인벤토리 꽉 찼을 때의 로직 구현
    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "InventoryItem")
	void OnInventoryFull(const APawn* Interactor);

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InventoryItem")
    EItemType ItemType;


protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InventoryItem")
	TObjectPtr<class UWidgetComponent> FullWidgetComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InventoryItem")
	TSubclassOf<class UUserWidget> FullWidgetClass;

	bool bInventoryFull = false;
};
