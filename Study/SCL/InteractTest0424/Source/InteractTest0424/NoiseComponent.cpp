// Fill out your copyright notice in the Description page of Project Settings.


#include "NoiseComponent.h"
#include "CameraObject.h"

// Sets default values for this component's properties
UNoiseComponent::UNoiseComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


void UNoiseComponent::MakeNoise()
{
	UE_LOG(LogTemp, Warning, TEXT("[%s] Noise Component Triggered - MaxRange: %f, Loudness: %f"),
		*GetOwner()->GetName(), NoiseSettings.MaxRange, NoiseSettings.Loudness);

	// AI 감지 시스템과 연결할 수 있는 위치
	// 실제 구현에서는 AIPerception 시스템과 연동
	ACameraObject* CameraOwner = GetOwner<ACameraObject>();
	ensure(CameraOwner);

	FVector ActualNoiseLocation = GetOwner()->GetActorLocation();

	// Makenoise 매개변수를 구조체로 묶어서 블루프린트에서 조절 가능하도록 빼기
	CameraOwner->Makenoise(
		NoiseSettings.Loudness,
		NoiseSettings.NoiseInstigator,
		ActualNoiseLocation + NoiseSettings.NoiseOffset,
		NoiseSettings.MaxRange,
		NoiseSettings.Tag);
}

// Called when the game starts
void UNoiseComponent::BeginPlay()
{
	Super::BeginPlay();
}