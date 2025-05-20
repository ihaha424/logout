// Fill out your copyright notice in the Description page of Project Settings.


#include "SzObjects/HackableObject.h"
#include "Blueprint/UserWidget.h"
#include "SzComponents/HackableComponent.h"

// Sets default values
AHackableObject::AHackableObject()
{
	PrimaryActorTick.bCanEverTick = false;

	bIsHacked = false;
}

// Called when the game starts or when spawned
void AHackableObject::BeginPlay()
{
	Super::BeginPlay();

	// 인터랙션 컴포넌트 확인
	if (!HackingComp)
	{
		HackingComp = FindComponentByClass<UHackableComponent>();
		if (!HackingComp)
		{
			UE_LOG(LogTemp, Warning, TEXT("No HackingComp found on %s"), *GetName());
		}
	}
}

void AHackableObject::StartHacking_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("StartHacking"));

	if (bIsHacked) return;
	if (RequiredEnergy <= 0) return;

	// RequiredTime 동안 E키를 눌렀으면 bIsHacked = true
	// RequiredTime이 되기 전에 E키를 뗐으면 bIsHacked = false
	// RequiredTime 이 되는 동안 게이지 참

	
	//GetWorldTimerManager.SetTimer(HackingTimer, )
	bIsHacked = true;

	if (bIsHacked)
	{
		FinishHacking_Implementation();
	}
}

void AHackableObject::FinishHacking_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("FinishHacking"));
	RequiredEnergy--;

	// 해킹 컴포넌트 확인
	if (HackingComp)
	{
		HackingComp->Execute();
	}



	//// HackedDuration 시간이 지나면 다시 해킹 전 상태(bIsHacked = false)로 바꿈
	//GetWorldTimerManager.SetTimer(HackedTimer, )

}

bool AHackableObject::CanBeHacked_Implementation() const
{
	// 플레이어의 코어 에너지가 없으면 false(나중에는 player->EnergyNum <=0 으로 비교)
	if (RequiredEnergy <= 0)
	{
		return false;
	}

	return !bIsHacked;	// 해킹된 상태랑 해킹할 수 있는 상태는 반대.
}

void AHackableObject::ClearHoldingTimer_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("ClearHoldingTimer"));
}

void AHackableObject::OnHackingSucceeded()
{

}

void AHackableObject::OnHackingExpired()
{

}
