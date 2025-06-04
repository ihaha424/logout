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
	AHackableObject();

protected:
    virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:
	// 해킹 실행 (E키 홀딩)
    virtual void OnHackingStarted_Implementation(APawn* Interactor) override;

	// 해킹 완료 후 로직 (CCTV 보임, 적 무력화 등)
	virtual void OnHackingCompleted_Implementation(APawn* Interactor) override;
    
	// 해킹 가능 여부 체크 (false => 해킹 전 / true => 해킹 완료)
	virtual bool CanBeHacked_Implementation() const override;

	// 해킹 초기화(해킹 안 된 상태로 만들기)
	virtual void ClearHacking_Implementation() override;

private:
	// Tick에서 호출: 해킹 진행 상태 업데이트
	void UpdateHackingProgress(float CurrentTime);

	// HeldTime이 조건을 만족하면 해킹 완료 처리
	void TryCompleteHacking(float HeldDuration, float CurrentTime);

	// 해킹 성공 후 일정 시간이 지나면 다시 해킹 가능 상태로 초기화
	void CheckHackReset(float CurrentTime);


protected:
	UPROPERTY(EditAnywhere, Category = "Hacking")
	TObjectPtr<class UHackableComponent> HackingComp;

    UPROPERTY(EditDefaultsOnly, Category = "Hacking")
    TSubclassOf<class UUserWidget> HackingGaugeWidget;

	// 해킹 게이지 위젯
	UPROPERTY()
	TObjectPtr<class UUserWidget> GuageUI;

	// 해킹 완료 후 유지 시간
	UPROPERTY(EditAnywhere, Category = "Hacking")
	float HackedDuration = 5.0f;

	// 해킹 키 홀딩 시간
	UPROPERTY(EditAnywhere, Category = "Hacking")
	float RequiredTime = 2.0f;

	// 해킹 키를 누르고 있는지
	UPROPERTY(BlueprintReadOnly)
	bool bIsHacking = false;

	// 해킹 상태
	UPROPERTY(BlueprintReadOnly)
	bool bIsHacked = false;

private:
	float HackingStartTime = 0.0f;
	bool bAutoHackingCompleted = false;
};

// 코어 에너지 사용 부분은 player에서 담당
// RequiredTime 이랑 IA_Hacking의 HoldTimeThreshold랑 같아야 함