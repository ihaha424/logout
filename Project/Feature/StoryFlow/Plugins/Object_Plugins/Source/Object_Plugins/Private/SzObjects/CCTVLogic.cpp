// Fill out your copyright notice in the Description page of Project Settings.


#include "SzObjects/CCTVLogic.h"
#include "SzComponents/CCTVManager.h"
#include "SzObjects/CCTV.h"
#include "PhantomTwinsGameState.h"
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

void ACCTVLogic::EnterFirstHackedCCTV(APawn* Interactor)
{
	if (CurrentGameState && CurrentCCTVManager)
	{
		ACCTV* firstCCTV = CurrentCCTVManager->GetFirstHackedCCTV();
		APlayerController* PC = CastChecked<APlayerController>(Interactor->GetController());

		if (firstCCTV)
		{
			// CCTV 진입
			firstCCTV->EnterCCTVView(PC);
		}
		else
		{
			// Error UI 토글
			if (CCTVWidgetInstance && CCTVWidgetInstance->IsInViewport())
			{
				UE_LOG(LogTemp, Log, TEXT("Error UI 제거"));
				CCTVWidgetInstance->RemoveFromParent();
				CCTVWidgetInstance = nullptr;
			}
			else
			{
				UE_LOG(LogTemp, Log, TEXT("Error UI 띄움"));

				if (NoHackedCCTVWidget)
				{
					CCTVWidgetInstance = CreateWidget<UUserWidget>(PC, NoHackedCCTVWidget);
					if (CCTVWidgetInstance)
					{
						CCTVWidgetInstance->AddToViewport();
					}
				}
			}
		}
	}
}