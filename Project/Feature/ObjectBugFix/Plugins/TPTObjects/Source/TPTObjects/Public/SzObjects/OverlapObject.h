// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SzObjects/BaseObject.h"
#include "GameplayTagContainer.h"
#include "ActiveGameplayEffectHandle.h"
#include "OverlapObject.generated.h"

/**
 * 
 */
UCLASS()
class TPTOBJECTS_API AOverlapObject : public ABaseObject
{
	GENERATED_BODY()

public:
	AOverlapObject();

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	UFUNCTION()
    void OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);

    UFUNCTION()
    void OnTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	// 아이템에 대한 자체 이펙트(상자 오픈 이펙트)를 재생하기 위한 함수
	UFUNCTION()
	void InvokeGameplayCue(AActor* Interactor);

	// Overlap 되었을 때 실행할 로직을 블루프린트에서 구현
    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "OverlapObject")
	void ReceiveBeginOverlap(AActor* Interactor);

	// Overlap 끝났을 때 실행할 로직을 블루프린트에서 구현
    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "OverlapObject")
	void ReceiveEndOverlap(AActor* Interactor);

	// Activate 로직을 블루프린트에서 구현
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "OverlapObject")
	void Activate();

	// Deactivate 로직을 블루프린트에서 구현
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "OverlapObject")
	void Deactivate(float DisableDuration);


public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "OverlapObject | Control")
	bool bEnableEffectAndCue = true;

protected:
	// 실제로 GE를 적용/해제할 때 쓰는 핸들 저장 맵
	UPROPERTY()
	TMap<AActor*, FActiveGameplayEffectHandle> ActiveEffectHandles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= "OverlapObject")
	TObjectPtr<class UBoxComponent> BoxTrigger;

	UPROPERTY(EditAnywhere, Category = "OverlapObject | GAS")
	TSubclassOf<class UGameplayEffect> GameplayEffectClass;

	UPROPERTY(EditAnywhere, Category = "OverlapObject | GAS", Meta=(Categories=GameplayCue))
	FGameplayTag GameplayCueTag;
};
