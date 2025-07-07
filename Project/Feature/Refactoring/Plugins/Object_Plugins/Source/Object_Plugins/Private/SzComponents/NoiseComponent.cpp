// Fill out your copyright notice in the Description page of Project Settings.


#include "SzComponents/NoiseComponent.h"
#include "Net/UnrealNetwork.h"
#include "Gameframework/Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

UNoiseComponent::UNoiseComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);

	bNoise = false;
}

void UNoiseComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UNoiseComponent, bNoise);
}

void UNoiseComponent::StartNoise()
{
	// 소음 시작
	bNoise = true;

	if (!GetWorld()) return;

	GetWorld()->GetTimerManager().SetTimer(
		NoiseTimerHandle,
		this,
		&UNoiseComponent::GenerateNoise,
		noisePeriod,
		true // 반복
	);
}

void UNoiseComponent::StopNoise()
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(NoiseTimerHandle);
	}

	bNoise = false;
}

void UNoiseComponent::GenerateNoise()
{
	if (!bNoise) return;

	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("UNoiseComponent::GenerateNoise - Owner is not a Pawn!"));
		return;
	}

	OwnerPawn->MakeNoise(
		static_cast<float>(noisePoint),
		OwnerPawn,
		OwnerPawn->GetActorLocation(),
		noiseRange
	);

}