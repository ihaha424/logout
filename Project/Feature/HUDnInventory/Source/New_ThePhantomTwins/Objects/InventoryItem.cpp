// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryItem.h"

AInventoryItem::AInventoryItem()
{
}

void AInventoryItem::OnInteractServer_Implementation(const APawn* Interactor)
{
	InvokeGameplayCue(Interactor);			// 자기 자신 이펙트 재생
	
	// 플레이어한테 EItemType 넘김.
	UE_LOG(LogTemp, Log, TEXT("플레이어한테 EItemType 넘김"));

	DestroyItem();
}
