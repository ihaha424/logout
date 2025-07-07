// Fill out your copyright notice in the Description page of Project Settings.


#include "SzObjects/HackableObject.h"
#include "Blueprint/UserWidget.h"
#include "Components/WidgetComponent.h"
#include "SzComponents/HackableComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "SzUI/HackingGauge.h"

AHackableObject::AHackableObject() : ABaseObject()
{
	PrimaryActorTick.bCanEverTick = true;

	HackingComp = CreateDefaultSubobject<UHackableComponent>(TEXT("HackableComponent"));

	CurrentHackingPawn = nullptr;
}

void AHackableObject::BeginPlay()
{
	Super::BeginPlay();
}

void AHackableObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!HackingComp) return;

	const float CurrentTime = GetWorld()->GetTimeSeconds();

	// 현재 해킹 중인 플레이어가 있을 때만 UpdateHackingProgress 호출
	if (CurrentHackingPawn && HackingComp->bIsHacking)
	{
		HackingComp->UpdateHackingProgress(CurrentHackingPawn, CurrentTime);
	}


	// 해킹된 상태에서 유지 시간이 지나면 초기화 (단, bKeepHacked가 false일 때만)
	if (HackingComp->bIsHacked && !HackingComp->bKeepHacked &&
		(CurrentTime - HackingComp->HackingStartTime >= HackingComp->HackedDuration))
	{
		HackingComp->CheckHackReset(CurrentHackingPawn);
		CurrentHackingPawn = nullptr; // 해킹이 리셋되면 현재 해킹 플레이어도 초기화
	}
}

void AHackableObject::OnHackingStartedServer_Implementation(const APawn* Interactor)
{
	UE_LOG(LogTemp, Log, TEXT("AHackableObject::OnHackingStarted Server"));

	if (!HackingComp || !Interactor) return;

	// 현재 해킹 중인 플레이어 저장
	CurrentHackingPawn = Interactor;

	// HackingComponent에 Interactor 전달
	HackingComp->HackingStarted(Interactor);
}

void AHackableObject::OnHackingCompletedServer_Implementation(const APawn* Interactor)
{
	UE_LOG(LogTemp, Log, TEXT("AHackableObject::OnHackingCompleted Server"));

	if (!HackingComp || !Interactor) return;

	// 해킹을 시작한 플레이어와 완료하는 플레이어가 같은지 확인
	if (CurrentHackingPawn != Interactor) return;

	// HackingComponent에 Interactor 전달
	HackingComp->HackingCompleted(Interactor);

	// 해킹 완료 후 현재 해킹 플레이어 초기화
	if (!HackingComp->bIsHacked)
	{
		CurrentHackingPawn = nullptr;
	}
}

bool AHackableObject::CanBeHacked_Implementation(const APawn* Interactor)
{
	SetWidgetVisible(bCanInteract);

	return !(HackingComp->bIsHacked) && !(HackingComp->bIsHacking);	// 해킹된 상태랑 해킹할 수 있는 상태는 반대.
}

void AHackableObject::ClearHacking_Implementation(const APawn* Interactor)
{
	// 해킹 초기화
	HackingComp->CheckHackReset();
	CurrentHackingPawn = nullptr;
}
