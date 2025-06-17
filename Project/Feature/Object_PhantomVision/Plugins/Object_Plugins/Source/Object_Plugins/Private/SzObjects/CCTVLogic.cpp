#include "SzObjects/CCTVLogic.h"
#include "SzComponents/CCTVManager.h"
#include "SzObjects/CCTV.h"
#include "PhantomTwinsGameState.h"
#include "GameFramework/PlayerController.h"
#include "Blueprint/UserWidget.h"

// Sets default values
ACCTVLogic::ACCTVLogic()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
}

// Called when the game starts or when spawned
void ACCTVLogic::BeginPlay()
{
	Super::BeginPlay();
	CurrentGameState = CastChecked<APhantomTwinsGameState>(GetWorld()->GetGameState());
	CurrentCCTVManager = CurrentGameState->GetCCTVManager();
	check(CurrentCCTVManager);
}

void ACCTVLogic::EnterFirstHackedCCTV(APawn* Interactor)
{
	if (!CurrentGameState || !CurrentCCTVManager) return;

	// PlayerController 획득 및 소유자 설정
	APlayerController* PC = CastChecked<APlayerController>(Interactor->GetController());
	SetOwner(PC);  // 이후 Client RPC가 이 PC에게 전달됨[2]

	ACCTV* FirstCCTV = CurrentCCTVManager->GetFirstHackedCCTV();
	if (FirstCCTV)
	{
		FirstCCTV->EnterCCTVView(PC);

		if (bIsWidgetShown)
		{
			HideNoHackedCCTVUI_Client();
			bIsWidgetShown = false;
		}
	}
	else
	{
		// 토글하여 오류 UI 생성/제거
		if (bIsWidgetShown)
		{
			HideNoHackedCCTVUI_Client();
			bIsWidgetShown = false;
		}
		else
		{
			ShowNoHackedCCTVUI_Client();
			bIsWidgetShown = true;
		}
	}
}

void ACCTVLogic::ShowNoHackedCCTVUI_Client_Implementation()
{
	// 소유자인 로컬 플레이어 컨트롤러 얻기
	APlayerController* LocalPC = Cast<APlayerController>(GetOwner());
	if (!LocalPC || !LocalPC->IsLocalPlayerController()) return;

	// 기존 위젯 제거  
	if (CCTVWidgetInstance && CCTVWidgetInstance->IsInViewport())
	{
		CCTVWidgetInstance->RemoveFromParent();
		CCTVWidgetInstance = nullptr;
	}

	// 새 위젯 생성 및 표시  
	if (NoHackedCCTVWidget)
	{
		CCTVWidgetInstance = CreateWidget<UUserWidget>(LocalPC, NoHackedCCTVWidget);
		if (CCTVWidgetInstance)
		{
			CCTVWidgetInstance->AddToViewport();
		}
	}
}

void ACCTVLogic::HideNoHackedCCTVUI_Client_Implementation()
{
	if (CCTVWidgetInstance && CCTVWidgetInstance->IsInViewport())
	{
		CCTVWidgetInstance->RemoveFromParent();
		CCTVWidgetInstance = nullptr;
	}
}
