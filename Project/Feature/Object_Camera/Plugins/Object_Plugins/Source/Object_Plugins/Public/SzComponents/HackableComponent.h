// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "HackableComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class OBJECT_PLUGINS_API UHackableComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHackableComponent();

public:	
	UFUNCTION(BlueprintCallable, Category = "Hacking")
	virtual void HackingStarted();

	UFUNCTION(BlueprintCallable, Category = "Hacking")
	virtual void HackingCompleted();

	void UpdateHackingProgress(float CurrentTime);
	void TryCompleteHacking(float HeldDuration, float CurrentTime);

	// 해킹 성공 후 일정 시간이 지나면 다시 해킹 가능 상태로 초기화
	void CheckHackReset();

public:
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

	float HackingStartTime = 0.0f;
	bool bAutoHackingCompleted = false;
};
