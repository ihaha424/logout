// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "NoiseComponent.generated.h"

// 소음만 발생
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class OBJECT_PLUGINS_API UNoiseComponent : public USceneComponent
{
	GENERATED_BODY()
	
public:
	UNoiseComponent();

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


public:
	void StartNoise();		// 소음 타이머 시작
	void StopNoise();		// 소음 타이머 정지

private:
	void GenerateNoise();	// 타이머로 반복 호출되는 함수

public:
	// 발생하는 소음 포인트
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise")
	int32 noisePoint = 1;

	// 소음 발생 범위
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise")
	float noiseRange = 50.0f;

	// 소음 발생 주기 (초마다)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise")
	float noisePeriod = 3.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Noise", Replicated)
	bool bNoise = false;

	FTimerHandle NoiseTimerHandle;
};
