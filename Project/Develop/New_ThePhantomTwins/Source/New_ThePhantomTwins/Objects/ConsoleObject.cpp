// Fill out your copyright notice in the Description page of Project Settings.


#include "ConsoleObject.h"
#include "Net/UnrealNetwork.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Blueprint/UserWidget.h"
#include "player/PlayerCharacter.h"
#include "Log/TPTLog.h"
#include "Door.h"
#include "Components/DecalComponent.h"


AConsoleObject::AConsoleObject() : AInteractableObject()
{
	bReplicates = true;

	// player 체크 할 trigger
	Trigger = CreateDefaultSubobject<USphereComponent>(TEXT("TriggerComponent"));
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
	if (!Interactor->IsLocallyControlled()) return bIsDetected;

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
	if (!bCanInteract) return;
	if (!AreAllTriggerActived() || HasPlayerNum != MaxPlayerNum) return;

	// 여기서 3초 지났는지 확인

	//TPT_LOG(ObjectLog, Log, TEXT("AConsoleObject :: OnInteractServer"));

	bIsActived = true;

	// 연결된 Door에 상태 변경 알림
	if (HasAuthority())
	{
		if (ConnectedDoor && bIsActived)
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

	// 아웃라인 코드
	if (bIsActived)
	{
		ShowOverlayOutline(!bIsActived);
	}

	if (!bVisible || bIsActived)
	{
		// 감지 안 되었거나 명시적으로 숨길 때는 둘 다 숨김
		InteractWidgetComp->SetVisibility(false);
		LockWidgetComp->SetVisibility(false);

		return;
	}

	// AreAllTriggerActived 와 HasPlayerNum 조건을 체크해서 InteractWidgetComp 표시 결정
	const bool bAllTriggersActive = AreAllTriggerActived();
	const bool bHasEnoughPlayers = (HasPlayerNum == MaxPlayerNum);

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
	// 1. APlayerCharacter인지 체크
	APlayerCharacter* PlayerChar = Cast<APlayerCharacter>(OtherActor);
	if (!PlayerChar) return;

	// 이미 들어와 있다면 무시
	if (OverlappingPlayers.Contains(PlayerChar))
	{
		return;
	}

	OverlappingPlayers.Add(PlayerChar);

	HasPlayerNum = FMath::Clamp(HasPlayerNum + 1, 0, MaxPlayerNum);
	//TPT_LOG(ObjectLog, Log, TEXT("Enter :: %s, HasPlayerNum = %d"), *PlayerChar->GetName(), HasPlayerNum);

	bCanInteract = (AreAllTriggerActived() && HasPlayerNum == MaxPlayerNum);
}

void AConsoleObject::OnTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	APlayerCharacter* PlayerChar = Cast<APlayerCharacter>(OtherActor);
	if (!PlayerChar) return;

	if (OverlappingPlayers.Contains(PlayerChar))
	{
		OverlappingPlayers.Remove(PlayerChar);
		HasPlayerNum = FMath::Clamp(HasPlayerNum - 1, 0, MaxPlayerNum);
		//TPT_LOG(ObjectLog, Log, TEXT("Exit :: %s, HasPlayerNum = %d"), *PlayerChar->GetName(), HasPlayerNum);
	}

	bCanInteract = (AreAllTriggerActived() && HasPlayerNum == MaxPlayerNum);
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

	if (bIsActived)
	{
		ShowOverlayOutline(!bIsActived);
	}
}
