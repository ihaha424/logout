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
#include "GM_PhantomTwins.h"


AConsoleObject::AConsoleObject() : AInteractableObject()
{
	bReplicates = true;

	// player 체크 할 trigger
	SafeZoneTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("SafeZoneTriggerComponent"));
	SafeZoneTrigger->SetCollisionProfileName(TEXT("OverlapAll"));
	SafeZoneTrigger->SetGenerateOverlapEvents(true);
	SafeZoneTrigger->SetupAttachment(RootSceneComp);

	// Console 위에 뜨는 3D 위젯
	WaitingPlayerWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("WaitingPlayerWidget"));
	WaitingPlayerWidgetComp->SetupAttachment(RootComponent);
	WaitingPlayerWidgetComp->SetWidgetSpace(EWidgetSpace::World);
}

void AConsoleObject::BeginPlay()
{
	Super::BeginPlay();

	if (WaitingPlayerWidgetComp)
	{
		WaitingPlayerWidgetComp->SetVisibility(false);
	}

	if (SafeZoneTrigger)
	{
		SafeZoneTrigger->OnComponentBeginOverlap.AddDynamic(this, &AConsoleObject::OnTriggerBeginOverlap);
		SafeZoneTrigger->OnComponentEndOverlap.AddDynamic(this, &AConsoleObject::OnTriggerEndOverlap);
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


	// 2D 위젯
	// GameMode 접근해서 레벨에 접근
	// 서버에서만 실행: 각 플레이어 컨트롤러에 Client RPC로 위젯 등록 요청

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PC = It->Get();
		if (!PC) continue;

		APC_Player* MyPC = Cast<APC_Player>(PC);
		if (MyPC)
		{
			if (ExitTimerWidgetClass)
			{
				// 위젯 등록 및 캐싱
				MyPC->RegisterWidget(TEXT("ExitTimerWidget"),
					CreateWidget<UUserWidget>(MyPC, ExitTimerWidgetClass));

				ExitTimerWidget = Cast<UUserWidget>(
					MyPC->GetWidget(TEXT("ExitTimerWidget")));
			}

			if (ElseExitTimerWidgetClass)
			{
				MyPC->RegisterWidget(TEXT("ElseExitTimerWidget"),
					CreateWidget<UUserWidget>(MyPC, ElseExitTimerWidgetClass));

				ElseExitTimerWidget = Cast<UUserWidget>(
					MyPC->GetWidget(TEXT("ElseExitTimerWidget")));
			}
		}
	}
}

void AConsoleObject::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AConsoleObject, LevelDataFragments);
	DOREPLIFETIME(AConsoleObject, InteractPlayers);
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
	//TPT_LOG(ObjectLog, Log, TEXT("AConsoleObject :: OnInteractServer"));

	if (!bCanInteract) return;
	
	// 1. APlayerCharacter인지 체크
	APlayerCharacter* PlayerChar = Cast<APlayerCharacter>(const_cast<APawn*>(Interactor));
	if (!PlayerChar) return;

	// 이미 들어와 있다면 무시
	if (!InteractPlayers.Contains(PlayerChar))
	{
		InteractPlayers.Add(PlayerChar);
	}

	bIsActived = true;

	S2A_ShowWaitingPlayerWidget(true);

	if (APC_Player* PC_Player = Cast<APC_Player>(Interactor->GetController()))
	{
		// 팝업 위젯
		PC_Player->SetWidget(TEXT("ExitTimerWidget"), true, EMessageTargetType::LocalClient);
		PC_Player->SetWidget(TEXT("ElseExitTimerWidget"), true, EMessageTargetType::AllExceptSelf);
	}
}


void AConsoleObject::OnInteractClient_Implementation(const APawn* Interactor)
{
	Super::OnInteractClient_Implementation(Interactor);

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
	}
	else
	{
		InteractWidgetComp->SetVisibility(true);
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
