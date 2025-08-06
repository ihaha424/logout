// Fill out your copyright notice in the Description page of Project Settings.


#include "ConsoleObject.h"
#include "Net/UnrealNetwork.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "Blueprint/UserWidget.h"
#include "../player/PlayerCharacter.h"
#include "../Log/TPTLog.h"
#include "Door.h"

AConsoleObject::AConsoleObject() : AInteractableObject()
{
	bReplicates = true;

	// player 체크 할 trigger
	Trigger = CreateDefaultSubobject<UCapsuleComponent>(TEXT("TriggerComponent"));
	Trigger->SetCollisionProfileName(TEXT("OverlapAll"));
	Trigger->SetGenerateOverlapEvents(true);
	Trigger->SetupAttachment(RootSceneComp);

	// Lock Widget
	LockWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("LockWidget"));
	LockWidgetComp->SetupAttachment(RootComponent);
	LockWidgetComp->SetWidgetSpace(EWidgetSpace::Screen);
	LockWidgetComp->SetDrawSize(FVector2D(10, 10));
	LockWidgetComp->SetRelativeLocation(FVector(0, 0, 100));
	LockWidgetComp->SetVisibility(false);
}

void AConsoleObject::BeginPlay()
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

	if (Trigger)
	{
		Trigger->OnComponentBeginOverlap.AddDynamic(this, &AConsoleObject::OnTriggerBeginOverlap);
		Trigger->OnComponentEndOverlap.AddDynamic(this, &AConsoleObject::OnTriggerEndOverlap);
	}
}

void AConsoleObject::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AConsoleObject, HasPlayerNum);
}

bool AConsoleObject::CanInteract_Implementation(const APawn* Interactor, bool bIsDetected)
{
	if (!Interactor->IsLocallyControlled()) return false;

	// 거리가 멀어져 감지되지 않은 경우
	if (!bIsDetected)
	{
		bCanInteract = false;
		SetWidgetVisible(false);
		return false;
	}

	// Interactor가 APlayerCharacter 인 경우에만 위젯을 띄어라
	const APlayerCharacter* PlayerChar = Cast<APlayerCharacter>(const_cast<APawn*>(Interactor));
	if (PlayerChar)
	{
		SetWidgetVisible(true);
	}

	return true;
}

void AConsoleObject::OnInteractServer_Implementation(const APawn* Interactor)
{
    // 여기서 3초 지났는지 확인
	TPT_LOG(ObjectLog, Log, TEXT("AConsoleObject :: OnInteractServer"));

	bIsActived = true;

	// 연결된 Door에 상태 변경 알림
	if (HasAuthority())
	{
		if (ConnectedDoor)
		{
			ConnectedDoor->CheckAndUpdateDoorState();
			ConnectedDoor->bCanInteract = false;
		}
	}

	bCanInteract = false;
}

void AConsoleObject::SetWidgetVisible(bool bVisible)
{
	if (!InteractWidgetComp || !LockWidgetComp) return;

	if (!bVisible || bIsActived)
	{
		// 감지 안 되었거나 명시적으로 숨길 때는 둘 다 숨김
		InteractWidgetComp->SetVisibility(false);
		LockWidgetComp->SetVisibility(false);
		return;
	}


	// AreAllTriggerActived 와 HasPlayerNum 조건을 체크해서 InteractWidgetComp 표시 결정
	const bool bAllTriggersActive = AreAllTriggerActived();
	const bool bHasEnoughPlayers = (HasPlayerNum == 2);

	if (bAllTriggersActive && bHasEnoughPlayers)
	{
		// 모두 모였을 때는 InteractWidgetComp만 노출
		InteractWidgetComp->SetVisibility(true);
		LockWidgetComp->SetVisibility(false);
	}
	else
	{
		// 조건 만족 못 하면 InteractWidgetComp 숨기고 LockWidgetComp 노출
		InteractWidgetComp->SetVisibility(false);
		LockWidgetComp->SetVisibility(true);
	}
}

void AConsoleObject::OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	HasPlayerNum = FMath::Clamp(HasPlayerNum + 1, 0, 2);

	if (AreAllTriggerActived() && HasPlayerNum == 2)
	{
		bCanInteract = true;
	}
	else
	{
		bCanInteract = false;
	}
}

void AConsoleObject::OnTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	HasPlayerNum = FMath::Clamp(HasPlayerNum - 1, 0, 2);

	if (AreAllTriggerActived() && HasPlayerNum == 2)
	{
		bCanInteract = true;
	}
	else
	{
		bCanInteract = false;
	}
}

bool AConsoleObject::AreAllTriggerActived() const
{
	// 활성화된 trigger 수를 세기 위한 변수
	int32 triggerActive = 0;

	// ConsoleObject와 연결된 모든 trigger를 하나씩 순회
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

void AConsoleObject::OnRep_bIsActived()
{
	if (!HasAuthority())
	{
		if (ConnectedDoor)
		{
			ConnectedDoor->CheckAndUpdateDoorState();
		}
	}
}
