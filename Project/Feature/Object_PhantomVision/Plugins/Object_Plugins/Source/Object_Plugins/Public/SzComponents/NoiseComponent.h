// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SzComponents/HackableComponent.h"
#include "NoiseComponent.generated.h"

/**
 * 
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class OBJECT_PLUGINS_API UNoiseComponent : public UHackableComponent
{
	GENERATED_BODY()
	
public:
	UNoiseComponent();

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


public:
	virtual void TryCompleteHacking(APawn* Interactor, float HeldDuration, float CurrentTime) override;
	virtual void CheckHackReset(APawn* Interactor) override;

	// 소음 타이머 시작
	void StartNoise();

	// 소음 타이머 정지
	void StopNoise();

private:
	// 타이머로 반복 호출되는 함수
	void GenerateNoise();

public:
	// 발생하는 소음 포인트
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hacking|Noise")
	int32 noisePoint = 1;

	// 소음 발생 범위
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hacking|Noise")
	float noiseRange = 50.0f;

	// 소음 발생 주기 (초마다)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hacking|Noise")
	float noisePeriod = 3.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Hacking|Noise", Replicated)
	bool bNoise = false;

	FTimerHandle NoiseTimerHandle;
};
