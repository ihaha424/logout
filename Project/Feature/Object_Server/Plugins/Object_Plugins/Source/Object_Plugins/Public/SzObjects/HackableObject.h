// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SzInterface/Hacking.h"
#include "HackableObject.generated.h"

UCLASS()
class OBJECT_PLUGINS_API AHackableObject : public AActor, public IHacking
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHackableObject();

protected:
    virtual void BeginPlay() override;

public:
	// 해킹 실행 (E키 홀딩)
    virtual void StartHacking_Implementation() override;
	
	// 해킹 완료 후 로직 (CCTV 보임, 적 무력화 등)
	virtual void FinishHacking_Implementation() override;
    
	// 해킹 가능 여부 체크 (false => 해킹 전 / true => 해킹 완료)
	virtual bool CanBeHacked_Implementation() const override;

	// 해킹 타이머 클리어
	virtual void ClearHoldingTimer_Implementation();

protected:
	//UPROPERTY(EditAnywhere, Category = "Hacking")
	//TObjectPtr<class UHackableComponent> HackingComp;

	// [임시용] 해킹에 필요한 코어 에너지
	UPROPERTY(EditAnywhere, Category = "Hacking")
	int32 RequiredEnergy = 3;

	// 해킹 유지 시간
	UPROPERTY(EditAnywhere, Category = "Hacking")
	float HackedDuration = 5.0f;

	// 해킹 키 홀딩 시간
	UPROPERTY(EditAnywhere, Category = "Hacking")
	float RequiredTime = 2.0f;

	// 해킹 상태
	UPROPERTY(BlueprintReadOnly)
	bool bIsHacked = false;

private:
	FTimerHandle HackingTimer;	// 해킹 중 타이머
	FTimerHandle HackedTimer;	// 해킹 완료 후 타이머

	void OnHackingSucceeded();
	void OnHackingExpired();
};
