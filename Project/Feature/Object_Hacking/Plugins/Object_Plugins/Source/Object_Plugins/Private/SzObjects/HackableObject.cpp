// Fill out your copyright notice in the Description page of Project Settings.


#include "SzObjects/HackableObject.h"
#include "Blueprint/UserWidget.h"
#include "SzComponents/HackableComponent.h"
#include "SzUI/HackingGauge.h"

// Sets default values
AHackableObject::AHackableObject()
{
	PrimaryActorTick.bCanEverTick = true;
	
	bIsHacking = false;
	bIsHacked = false;
	HackingStartTime = 0.0f;
	GuageUI = nullptr;
}

// Called when the game starts or when spawned
void AHackableObject::BeginPlay()
{
	Super::BeginPlay();

	// 인터랙션 컴포넌트 확인
	if (!HackingComp)
	{
		HackingComp = FindComponentByClass<UHackableComponent>();
		if (!HackingComp)
		{
			UE_LOG(LogTemp, Warning, TEXT("No HackingComp found on %s"), *GetName());
		}
	}

	// 게이지 위젯 설정

}

void AHackableObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsHacking && GuageUI)
	{
		// 누르고 있는 시간 계산
		float HeldDuration = GetWorld()->GetTimeSeconds() - HackingStartTime;

		// Cast 후 시간 전달
		if (UHackingGauge* Widget = Cast<UHackingGauge>(GuageUI))
		{
			Widget->UpdateHoldTime(HeldDuration);
		}
	}
}

void AHackableObject::OnHackingStarted_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("Hacking : Started"));

	// 해킹 되어있으면 return
	if (bIsHacked) return;

	// 현재 시간을 저장 (시작 시간)
	HackingStartTime = GetWorld()->GetTimeSeconds();
	bIsHacking = true;


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

void AHackableObject::OnHackingCompleted_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("FinishHacking"));

	// 해킹 중 상태 초기화
	HackingStartTime = 0.0f;
	bIsHacking = false;

	// UI 제거
	if (GuageUI)
	{
		GuageUI->RemoveFromParent();
		GuageUI = nullptr;
	}

	// 누르고 있었던 총 시간 계산
	float HeldDuration = GetWorld()->GetTimeSeconds() - HackingStartTime;
	UE_LOG(LogTemp, Warning, TEXT("Interact Held Duration: %.2f seconds"), HeldDuration);

	// 누르고 있던 시간이 RequiredTime 보다 크면
	if (HeldDuration >= RequiredTime)
	{
		// 해킹 컴포넌트 확인
		if (HackingComp)
		{
			HackingComp->Execute();
		}
		
		// 해킹 성공
		bIsHacked = true;
	}
	else
	{
		// 해킹 실패
		bIsHacked = false;
	}	

	// 
	// ToDo::해킹 성공 후 HackedDuration 시간이 지나면 다시 해킹 전 상태(bIsHacked = false)로 바꿈


}

bool AHackableObject::CanBeHacked_Implementation() const
{
	return !bIsHacked;	// 해킹된 상태랑 해킹할 수 있는 상태는 반대.
}