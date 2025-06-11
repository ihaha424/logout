// Fill out your copyright notice in the Description page of Project Settings.


#include "SzComponents/NoiseComponent.h"
#include "Gameframework/Actor.h"
#include "Gameframework/Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

UNoiseComponent::UNoiseComponent() : UHackableComponent()
{
	bNoise = false;
}

void UNoiseComponent::TryCompleteHacking(float HeldDuration, float CurrentTime)
{
	Super::TryCompleteHacking(HeldDuration, CurrentTime);

	UE_LOG(LogTemp, Log, TEXT("UNoiseComponent::TryCompleteHacking"));

	StartNoise();
}

void UNoiseComponent::CheckHackReset()
{
	Super::CheckHackReset();

	StopNoise();
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

	UE_LOG(LogTemp, Log, TEXT("UNoiseComponent::GenerateNoise - Making noise at location."));
}