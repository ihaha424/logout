// Fill out your copyright notice in the Description page of Project Settings.


#include "SzComponents/HackableComponent.h"
#include "Blueprint/UserWidget.h"
#include "SzUI/HackingGauge.h"

// Sets default values for this component's properties
UHackableComponent::UHackableComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	bIsHacking = false;
	bIsHacked = false;
	HackingStartTime = 0.0f;
	GuageUI = nullptr;
	bAutoHackingCompleted = false;
}

void UHackableComponent::HackingStarted()
{
	// 해킹 되어있거나 해킹 중이면 return
	if (bIsHacked || bIsHacking) return;

	UE_LOG(LogTemp, Log, TEXT("Hacking : Started"));

	// 현재 시간을 저장 (시작 시간)
	bIsHacking = true;
	bAutoHackingCompleted = false;
	HackingStartTime = GetWorld()->GetTimeSeconds();

	// 이미 UI가 있으면 제거
	if (GuageUI)
	{
		GuageUI->RemoveFromParent();
		GuageUI = nullptr;
	}

	// 새 UI 위젯 생성 및 화면에 추가
	if (HackingGaugeWidget)
	{
		GuageUI = CreateWidget<UUserWidget>(GetWorld(), HackingGaugeWidget);
		if (GuageUI)
		{
			GuageUI->AddToViewport(); // UI를 화면에 표시
		}
	}
}

void UHackableComponent::HackingCompleted()
{
	if (!bIsHacking || bAutoHackingCompleted) return;

	const float CurrentTime = GetWorld()->GetTimeSeconds();
	const float HeldDuration = CurrentTime - HackingStartTime;

	if (HeldDuration >= RequiredTime)
	{
		// 이미 성공 처리된 경우는 무시 (이중 처리 방지)
		return;
	}
	else
	{
		// 실패 처리
		UE_LOG(LogTemp, Warning, TEXT("Hacking Failed: %.2f / %.2f"), HeldDuration, RequiredTime);
		CheckHackReset();
	}
}

void UHackableComponent::UpdateHackingProgress(float CurrentTime)
{
	if (bIsHacking && !bIsHacked && GuageUI)
	{
		float HeldDuration = CurrentTime - HackingStartTime;

		// 해킹중 UI 시간 업데이트
		if (UHackingGauge* Widget = Cast<UHackingGauge>(GuageUI))
		{
			Widget->UpdateHoldTime(HeldDuration);
		}

		// HeldTime이 충분하면 자동으로 해킹 성공 처리
		if (HeldDuration >= RequiredTime)
		{
			TryCompleteHacking(HeldDuration, CurrentTime);
			bAutoHackingCompleted = true;
		}
	}
}

void UHackableComponent::TryCompleteHacking(float HeldDuration, float CurrentTime)
{
	if (bIsHacked)
		return;

	UE_LOG(LogTemp, Log, TEXT("Hacking Success! %.2f / %.2f"), HeldDuration, RequiredTime);

	bIsHacked = true;
	bIsHacking = false;
	HackingStartTime = CurrentTime;

	if (UHackingGauge* Widget = Cast<UHackingGauge>(GuageUI))
	{
		Widget->ShowCompletedMessage(); // 해킹 완료 메시지 표시
	}
}

void UHackableComponent::CheckHackReset()
{
	bIsHacking = false;
	bIsHacked = false;
	HackingStartTime = 0.0f;
	bAutoHackingCompleted = false;

	if (GuageUI)
	{
		GuageUI->RemoveFromParent();
		GuageUI = nullptr;
	}

	UE_LOG(LogTemp, Log, TEXT("해킹 상태 초기화 완료"));
}
