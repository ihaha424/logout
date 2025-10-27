// Fill out your copyright notice in the Description page of Project Settings.


#include "ConsoleObject.h"
#include "Net/UnrealNetwork.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Blueprint/UserWidget.h"
#include "player/PlayerCharacter.h"
#include "player/PC_Player.h"
#include "Log/TPTLog.h"
#include "Door.h"
#include "Components/DecalComponent.h"
#include "Objects/DataFragment.h"
#include "Kismet/GameplayStatics.h"
#include "UI/Wait5SecondsWidget.h"
#include "GS_PhantomTwins.h"

AConsoleObject::AConsoleObject() : AInteractableObject()
{
	bReplicates = true;

	// player 체크 할 trigger
	SafeZoneTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("SafeZoneTriggerComponent"));
	SafeZoneTrigger->SetCollisionProfileName(TEXT("OverlapAll"));
	SafeZoneTrigger->SetGenerateOverlapEvents(true);
	SafeZoneTrigger->SetupAttachment(RootSceneComp);

	// Lock Widget
	LockWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("LockWidget"));
	LockWidgetComp->SetupAttachment(RootComponent);
	LockWidgetComp->SetWidgetSpace(EWidgetSpace::Screen);
	LockWidgetComp->SetDrawSize(FVector2D(10, 10));
	LockWidgetComp->SetRelativeLocation(FVector(0, 0, 100));

	// Console 위에 뜨는 3D 위젯
	WaitingPlayerWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("WaitingPlayerWidget"));
	WaitingPlayerWidgetComp->SetupAttachment(RootComponent);
	WaitingPlayerWidgetComp->SetWidgetSpace(EWidgetSpace::World);
}

void AConsoleObject::BeginPlay()
{
	Super::BeginPlay();

	if (SafeZoneTrigger)
	{
		SafeZoneTrigger->OnComponentBeginOverlap.AddDynamic(this, &AConsoleObject::OnTriggerBeginOverlap);
		SafeZoneTrigger->OnComponentEndOverlap.AddDynamic(this, &AConsoleObject::OnTriggerEndOverlap);
	}

	if (LockWidgetComp)
	{
		if (LockWidgetClass)
		{
			LockWidgetComp->SetWidgetClass(LockWidgetClass);
			LockWidgetComp->SetVisibility(false);
		}
	}

	if (WaitingPlayerWidgetComp)
	{
		WaitingPlayerWidgetComp->SetVisibility(false);
	}

	//	레벨에 존재하는 DataFragments들 찾아서 LevelDataFragments배열에 추가
	TArray<AActor*> FoundFragments;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADataFragment::StaticClass(), FoundFragments);
	LevelDataFragments.Empty();
	for (AActor* Actor : FoundFragments)
	{
		if (ADataFragment* DataFragment = Cast<ADataFragment>(Actor))
		{
			if (!LevelDataFragments.Contains(DataFragment))
			{
				LevelDataFragments.Add(DataFragment);
			}
		}
	}
}

void AConsoleObject::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AConsoleObject, LevelDataFragments);
	DOREPLIFETIME(AConsoleObject, InteractPlayers);
	DOREPLIFETIME(AConsoleObject, bCanUse);
}

bool AConsoleObject::CanInteract_Implementation(const APawn* Interactor, bool bIsDetected)
{
	if (!Interactor->IsLocallyControlled()) return bIsDetected;

	// 거리가 멀어져 감지되지 않은 경우
	if (!bIsDetected)
	{
		bCanUse = false;
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
	//TPT_LOG(ObjectLog, Log, TEXT("AConsoleObject :: OnInteractServer"));

	if (!bCanUse) return;
	
	// 1. APlayerCharacter인지 체크
	APlayerCharacter* PlayerChar = Cast<APlayerCharacter>(const_cast<APawn*>(Interactor));
	if (!PlayerChar) return;

	// 이미 들어와 있다면 무시
	if (!InteractPlayers.Contains(PlayerChar))
	{
		InteractPlayers.Add(PlayerChar);
	}

	bIsActived = true;

	S2A_ShowWaitingPlayerWidget(bCanUse);
}


void AConsoleObject::OnInteractClient_Implementation(const APawn* Interactor)
{
	Super::OnInteractClient_Implementation(Interactor);

	if (!bCanUse) return;

	if (APC_Player* PC_Player = Cast<APC_Player>(Interactor->GetController()))
	{
		// 팝업 위젯
		PC_Player->SetWidget(TEXT("Wait5Seconds"), true, EMessageTargetType::LocalClient);
		PC_Player->SetWidget(TEXT("AskExit"), true, EMessageTargetType::AllExceptSelf);
	}
}

void AConsoleObject::SetWidgetVisible(bool bVisible)
{
	if (!InteractWidgetComp) return;

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
		bCanUse = false;
	}

	// GS에서 OnCollectedItemCountChanged() 가져옴
	AGS_PhantomTwins* GS = Cast<AGS_PhantomTwins>(GetWorld()->GetGameState());
	int32 currDataFragments = 0;
	if (GS)
	{
		currDataFragments = GS->CoreCount;
	}

	// currDataFragments가 LevelDataFragments보다 작으면
	if (currDataFragments < LevelDataFragments.Num())
	{
		// 잠금 위젯
		InteractWidgetComp->SetVisibility(false);
		LockWidgetComp->SetVisibility(true);
		bCanUse = false;
	}
	else
	{
		// currDataFragments가 LevelDataFragments같거나 많으면 상호작용 위젯
		InteractWidgetComp->SetVisibility(true);
		LockWidgetComp->SetVisibility(false);
		bCanUse = true;
	}
}

void AConsoleObject::OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//// 1. APlayerCharacter인지 체크
	//APlayerCharacter* PlayerChar = Cast<APlayerCharacter>(OtherActor);
	//if (!PlayerChar) return;

	//// 이미 들어와 있다면 무시
	//if (!InteractPlayers.Contains(PlayerChar))
	//{
	//	InteractPlayers.Add(PlayerChar);
	//}
}

void AConsoleObject::OnTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	APlayerCharacter* PlayerChar = Cast<APlayerCharacter>(OtherActor);
	if (!PlayerChar) return;
	
	if (InteractPlayers.Contains(PlayerChar))
	{
		InteractPlayers.Remove(PlayerChar);
	}
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

void AConsoleObject::S2A_ShowWaitingPlayerWidget_Implementation(bool bVisible)
{
	// 모든 플레이어한테 3D UI WBP_WaitingPlayer 띄움
	if (WaitingPlayerWidgetComp)
	{
		WaitingPlayerWidgetComp->SetVisibility(bVisible);
	}
}

void AConsoleObject::OnRep_bCanUse()
{

}
