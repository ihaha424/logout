// Fill out your copyright notice in the Description page of Project Settings.


#include "Door.h"
#include "Net/UnrealNetwork.h"
#include "Components/WidgetComponent.h"
#include "Blueprint/UserWidget.h"
#include "../Player/PlayerCharacter.h"
#include "../Log/TPTLog.h"
#include "SaveGame/SaveIDComponent.h"
#include "SaveGame/TPTSaveGameManager.h"

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

	DOREPLIFETIME(ADoor, bKeyUsed);
}

bool ADoor::CanInteract_Implementation(const APawn* Interactor, bool bIsDetected)
{
	if (!Interactor->IsLocallyControlled()) return bIsDetected;
	// 거리가 멀어져 감지되지 않은 경우
	if (!bIsDetected)
	{
		bCanInteract = false;
		SetWidgetVisible(false);
		return false;
	}

	// 감지되었고 상호작용 가능한 상태
	//bCanInteract = true;

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
	// 모든 트리거가 활성화 되지 않으면 return
	if (!AreAllTriggerActived()) return;

	// Interactor가 APlayerCharacter 타입이 아니면 동작하지 않음 (문 닫기에만 해당)
	const APlayerCharacter* PlayerChar = Cast<APlayerCharacter>(const_cast<APawn*>(Interactor));

	// 문이 열려있는 상태에서 문을 닫으려 하는데, 플레이어 캐릭터가 아니면 종료
	if (bIsActived && !PlayerChar) return;

	bIsActived = !bIsActived;  // 상태 토글

	if (HasAuthority())
	{
		if (bIsActived)
		{
			S2A_OpenDoor();
		}
		else
		{
			S2A_CloseDoor();
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

void ADoor::CheckAndUpdateDoorState()
{
	// 모든 트리거가 활성화되었고 문이 아직 열리지 않았다면
	if (AreAllTriggerActived() && !bIsActived)
	{
		bIsActived = true;

		UTPTSaveGameManager* SaveGameManager = GetGameInstance()->GetSubsystem<UTPTSaveGameManager>();
		SaveGameManager->TempSaveByID(FindComponentByClass<USaveIDComponent>()->SaveId, true);

		S2A_OpenDoor();
	}
	// 트리거가 비활성화되었고 문이 열려있다면
	else if (!AreAllTriggerActived() && bIsActived)
	{
		bIsActived = false;
		S2A_CloseDoor();
	}
}

bool ADoor::AreAllTriggerActived_Implementation()
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

	if (triggerActive >= MinRequiredCount)
	{
		return true;
	}
	else
	{
		// 오브젝트가 만약 열쇠를 사용했다면 true
		if (ActorHasTag(TEXT("KeyInteract")) && bKeyUsed)
		{
			return true;
		}
		else
		{
			ShowLockedDoorDialog();
			return false;
		}
	}
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

void ADoor::OnRep_bKeyUsed()
{
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