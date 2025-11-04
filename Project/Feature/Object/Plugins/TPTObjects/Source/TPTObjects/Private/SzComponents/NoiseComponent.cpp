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

void UNoiseComponent::OnComponentDestroyed(bool bDestroyingHierarchy)
{
	Super::OnComponentDestroyed(bDestroyingHierarchy);

	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(NoiseTimerHandle);
	}
}

void UNoiseComponent::EndPlay(EEndPlayReason::Type Reason)
{
	GetWorld()->GetTimerManager().ClearTimer(NoiseTimerHandle);
	Super::EndPlay(Reason);
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
	if (!IsValid(OwnerPawn))
	{
		//UE_LOG(LogTemp, Warning, TEXT("UNoiseComponent::GenerateNoise - OwnerPawn is invalid or pending kill."));
		StopNoise();  // 소음 타이머 정지로 안전 종료도 고려
		return;
	}

	OwnerPawn->MakeNoise(
		static_cast<float>(noisePoint),
		OwnerPawn,
		OwnerPawn->GetActorLocation(),
		noiseRange,
		"NoiseItem"
	);

	//UE_LOG(LogTemp, Log, TEXT("UNoiseComponent::GenerateNoise noise generated"));
}