// Fill out your copyright notice in the Description page of Project Settings.


#include "SzObjects/HackableObject.h"

// Sets default values
AHackableObject::AHackableObject()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	bIsHacked = false;
}

// Called when the game starts or when spawned
void AHackableObject::BeginPlay()
{
	Super::BeginPlay();
	
}

void AHackableObject::StartHacking_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("StartHacking"));

	if (bIsHacked) return;

	// RequiredTime 동안 E키를 눌렀으면 bIsHacked = true
	// RequiredTime이 되기 전에 E키를 뗐으면 bIsHacked = false
}

void AHackableObject::FinishHacking_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("FinishHacking"));

	// 해킹 컴포넌트 확인
	//if (HackingComp)
	//{
	//	HackingComp->Execute();
	//}
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
