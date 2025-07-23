
#include "InventoryComponent.h"

UInventoryComponent::UInventoryComponent()
{

	PrimaryComponentTick.bCanEverTick = false;

}


void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	InventorySlots.Reserve(MaxInventorySlots);

}

void UInventoryComponent::AddItem(EItemType ItemType)
{

}

void UInventoryComponent::UseItem(int32 SlotIndex)
{

}
