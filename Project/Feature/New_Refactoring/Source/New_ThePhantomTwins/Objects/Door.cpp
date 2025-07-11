// Fill out your copyright notice in the Description page of Project Settings.


#include "Door.h"
#include "Net/UnrealNetwork.h"
#include "Components/WidgetComponent.h"
#include "Blueprint/UserWidget.h"

ADoor::ADoor() : AStaticObject()
{
	bReplicates = true;

	LockWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("LockWidget"));
	LockWidgetComp->SetupAttachment(RootComponent);
	LockWidgetComp->SetWidgetSpace(EWidgetSpace::Screen);
	LockWidgetComp->SetDrawSize(FVector2D(10, 10));
	LockWidgetComp->SetRelativeLocation(FVector(0, 0, 100));
	LockWidgetComp->SetVisibility(false);
}

void ADoor::BeginPlay()
{
	Super::BeginPlay();

	if (LockWidgetComp)
	{
		if (LockWidgetClass)
		{
			LockWidgetComp->SetWidgetClass(LockWidgetClass);
			LockWidgetComp->SetVisibility(false);
		}
	}

	// 필요한 활성화 수가 0이라면 트리거 개수 전체를 사용
	if (NeededActive == 0)
	{
		NeededActive = Triggers.Num();
	}
}

void ADoor::SetWidgetVisible(bool bVisible)
{
	if (!NearWidgetComp || !LockWidgetComp)
	{
		return;
	}

	if (!bVisible)
	{
		NearWidgetComp->SetVisibility(false);
		LockWidgetComp->SetVisibility(false);
		return;
	}

	// bVisible == true이고 트리거들이 활성화되지 않은 경우 → LockWidget
	if (!AreAllTriggerActived())
	{
		NearWidgetComp->SetVisibility(false);
		LockWidgetComp->SetVisibility(true);
	}
	else // 트리거들이 활성화된 경우 → NearWidget
	{
		NearWidgetComp->SetVisibility(true);
		LockWidgetComp->SetVisibility(false);
	}
}

void ADoor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

bool ADoor::CanInteract_Implementation(const APawn* Interactor, bool bIsDetected)
{
	if (!Interactor->IsLocallyControlled()) return false;
	
	// 거리가 멀어져 감지되지 않은 경우
	if (!bIsDetected)
	{
		bCanInteract = false;
		SetWidgetVisible(false);
		return false;
	}

	// 감지되었고 상호작용 가능한 상태
	bCanInteract = true;
	SetWidgetVisible(true);
	return true;
}

void ADoor::OnInteractServer_Implementation(const APawn* Interactor)
{
	// 모든 트리거가 활성화 되면
	if (AreAllTriggerActived())
	{
		// 문을 열어라
		S2A_OpenDoor();
	}
}

void ADoor::OnInteractClient_Implementation(const APawn* Interactor)
{
	if (AreAllTriggerActived())
	{
		NearWidgetComp->SetVisibility(false);
		LockWidgetComp->SetVisibility(false);
	}
}

bool ADoor::AreAllTriggerActived_Implementation() const
{	// 제거
	// 활성화된 trigger 수를 세기 위한 변수
	int32 triggerActive = 0;

	// Door와 연결된 모든 trigger를 하나씩 순회
	for (auto* trigger : Triggers)
	{
		AStaticObject* StaticTrigger = Cast<AStaticObject>(trigger);

		if (StaticTrigger && StaticTrigger->bActived)
		{
			triggerActive++;
		}
	}

	return triggerActive >= NeededActive;
}

void ADoor::S2A_OpenDoor_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("OpenDoor"));
}
