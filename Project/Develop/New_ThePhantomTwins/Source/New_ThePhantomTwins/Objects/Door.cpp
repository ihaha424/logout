// Fill out your copyright notice in the Description page of Project Settings.


#include "Door.h"
#include "Net/UnrealNetwork.h"
#include "Components/WidgetComponent.h"
#include "Blueprint/UserWidget.h"
#include "../Player/PlayerCharacter.h"
#include "../Log/TPTLog.h"

ADoor::ADoor() : AInteractableObject()
{
	LockWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("LockWidget"));
	LockWidgetComp->SetupAttachment(RootComponent);
	LockWidgetComp->SetWidgetSpace(EWidgetSpace::Screen);
	LockWidgetComp->SetDrawSize(FVector2D(10, 10));
	LockWidgetComp->SetRelativeLocation(FVector(0, 0, 100));
	LockWidgetComp->SetVisibility(false);
	//TPT_LOG(ObjectLog,Log,TEXT(""));
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
	if (MinRequiredCount == 0)
	{
		MinRequiredCount = RequiredList.Num();
	}
}

void ADoor::SetWidgetVisible(bool bVisible)
{
	if (!InteractWidgetComp || !LockWidgetComp)
	{
		return;
	}

	if (!bVisible)
	{
		InteractWidgetComp->SetVisibility(false);
		LockWidgetComp->SetVisibility(false);
		return;
	}

	// bVisible == true이고 트리거들이 활성화되지 않은 경우 → LockWidget
	if (!AreAllTriggerActived())
	{
		InteractWidgetComp->SetVisibility(false);
		LockWidgetComp->SetVisibility(true);
	}
	else // 트리거들이 활성화된 경우 → NearWidget
	{
		InteractWidgetComp->SetVisibility(true);
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
	
	// Interactor가 APlayerCharacter 인 경우에만 위젯을 띄어라
	const APlayerCharacter* PlayerChar = Cast<APlayerCharacter>(const_cast<APawn*>(Interactor));
	if (PlayerChar)
	{
		SetWidgetVisible(true);
	}

	return true;
}

void ADoor::OnInteractServer_Implementation(const APawn* Interactor)
{
	// 모든 트리거가 활성화 되면
	if (AreAllTriggerActived())
	{
		bIsActived = !bIsActived;

		if (HasAuthority())
		{
			if (bIsActived)
			{
				// 문을 열어라
				S2A_OpenDoor();
			}
			else
			{
				S2A_CloseDoor();
			}
		}
	}


}

void ADoor::OnInteractClient_Implementation(const APawn* Interactor)
{
	if (AreAllTriggerActived())
	{
		InteractWidgetComp->SetVisibility(false);
		LockWidgetComp->SetVisibility(false);
	}
}

bool ADoor::CanBeDestroyed_Implementation(const APawn* Interactor)
{
	// bIsActived = false면 부술 수 있음
	return !bIsActived;
}

bool ADoor::AreAllTriggerActived_Implementation() const
{	// 제거
	// 활성화된 trigger 수를 세기 위한 변수
	int32 triggerActive = 0;

	// Door와 연결된 모든 trigger를 하나씩 순회
	for (auto* trigger : RequiredList)
	{
		AInteractableObject* RequiredTrigger = Cast<AInteractableObject>(trigger);

		if (RequiredTrigger && RequiredTrigger->bIsActived)
		{
			triggerActive++;
		}
	}

	return triggerActive >= MinRequiredCount;
}

void ADoor::OnRep_bIsActived()
{
	if (AreAllTriggerActived())
	{
		if (bIsActived)
		{
			// 문을 열어라
			S2A_OpenDoor();
		}
		else
		{
			S2A_CloseDoor();
		}
	}

}

void ADoor::S2A_OpenDoor_Implementation()
{
	OpenDoor();
}

void ADoor::S2A_CloseDoor_Implementation()
{
	CloseDoor();
}
