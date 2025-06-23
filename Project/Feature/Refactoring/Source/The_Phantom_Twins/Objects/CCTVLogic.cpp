// Fill out your copyright notice in the Description page of Project Settings.


#include "CCTVLogic.h"
#include "CCTVManager.h"
#include "CCTV.h"
#include "../PhantomTwinsGameState.h"
#include "GameFramework/PlayerController.h"
#include "Blueprint/UserWidget.h"

// Sets default values
ACCTVLogic::ACCTVLogic()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void ACCTVLogic::BeginPlay()
{
	Super::BeginPlay();

	CurrentGameState = CastChecked<APhantomTwinsGameState>(GetWorld()->GetGameState());
	CurrentCCTVManager = CurrentGameState->GetCCTVManager();
	check(CurrentCCTVManager);
}

bool ACCTVLogic::EnterFirstHackedCCTV(APawn* Interactor)
{
	if (CurrentGameState && CurrentCCTVManager)
	{
		ACCTV* firstCCTV = CurrentCCTVManager->GetFirstHackedCCTV();
		APlayerController* PC = CastChecked<APlayerController>(Interactor->GetController());

		if (firstCCTV)
		{
			// CCTV 진입
			firstCCTV->EnterCCTVView(PC);
			return true;
		}
		else
		{
			return false;
		}
	}
	UE_LOG(LogCameraManger, Error, TEXT("ACCTVLogic: EnterFirstHackedCCTV: CurrentGameState || CurrentCCTVManager is nullptr."));
	return true;
}

void ACCTVLogic::SetWidget(APlayerController* PC)
{
	// Error UI 토글
	if (CCTVWidgetInstance && CCTVWidgetInstance->IsInViewport())
	{
		CCTVWidgetInstance->RemoveFromParent();
		CCTVWidgetInstance = nullptr;
	}
	else
	{
		if (NoHackedCCTVWidget)
		{
			if(!CCTVWidgetInstance)
				CCTVWidgetInstance = CreateWidget<UUserWidget>(PC, NoHackedCCTVWidget);
			if (CCTVWidgetInstance)
			{
				CCTVWidgetInstance->AddToViewport();
			}
		}
	}
}
