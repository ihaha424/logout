// Fill out your copyright notice in the Description page of Project Settings.


#include "SzComponents/NoiseComponent.h"
#include "Net/UnrealNetwork.h"
#include "Gameframework/Actor.h"
#include "Gameframework/Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Components/ActorComponent.h"

UNoiseComponent::UNoiseComponent() : UHackableComponent()
{
	bNoise = false;
	SetIsReplicatedByDefault(true);
}

void UNoiseComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UNoiseComponent, bNoise);
}

void UNoiseComponent::TryCompleteHacking(APawn* Interactor, float HeldDuration, float CurrentTime)
{
	Super::TryCompleteHacking(Interactor, HeldDuration, CurrentTime);

	UE_LOG(LogTemp, Log, TEXT("UNoiseComponent::TryCompleteHacking"));

	StartNoise();
}

void UNoiseComponent::CheckHackReset(APawn* Interactor)
{
	Super::CheckHackReset(Interactor);

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
	bNoise = false;

	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(NoiseTimerHandle);
	}
}

void UNoiseComponent::GenerateNoise()
{
	if (!bNoise) return;

	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn) return;

	OwnerPawn->MakeNoise(
		static_cast<float>(noisePoint),
		OwnerPawn,
		OwnerPawn->GetActorLocation(),
		noiseRange
	);
}
