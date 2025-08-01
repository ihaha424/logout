#include "DataFragment.h"
#include "Components/StaticMeshComponent.h"
#include "TimerManager.h"
#include "Blueprint/UserWidget.h"
#include "../Player/PC_Player.h"
#include "../UI/HUD/PlayerHUDWidget.h"
#include "../UI/DataFragmentPickupWidget.h"

ADataFragment::ADataFragment()
{
}

void ADataFragment::BeginPlay()
{
	Super::BeginPlay();

	if (APC_Player* PC_Player = Cast<APC_Player>(GetWorld()->GetFirstPlayerController()))
	{
		// 위젯 등록 및 캐싱
		PC_Player->RegisterWidget(TEXT("DataFragmentPickupWidget"),
			CreateWidget<UDataFragmentPickupWidget>(PC_Player, DataFragmentPickupWidgetClass));

		DataFragmentPickupWidget = Cast<UDataFragmentPickupWidget>(
			PC_Player->GetWidget(TEXT("DataFragmentPickupWidget")));
	}
}

void ADataFragment::OnInteractServer_Implementation(const APawn* Interactor)
{
	bIsActived = true;

	InvokeGameplayCue(Interactor);
	ApplyEffectToTarget(Interactor);

	SetDataFragmentPickupWidget();
}

void ADataFragment::SetDataFragmentPickupWidget()
{
	HideFragmentMesh();
	SetActorEnableCollision(false);

	if (APC_Player* PC_Player = Cast<APC_Player>(GetWorld()->GetFirstPlayerController()))
	{
		PC_Player->SetWidget(TEXT("DataFragmentPickupWidget"), true, EMessageTargetType::Multicast);

		UDataFragmentPickupWidget* Widget = Cast<UDataFragmentPickupWidget>(
			PC_Player->GetWidget(TEXT("DataFragmentPickupWidget")));
		if (Widget)
		{
			Widget->SetText(DataFragmentText);
		}

		FTimerHandle WidgetTimerHandle;
		GetWorld()->GetTimerManager().SetTimer(
			WidgetTimerHandle,
			FTimerDelegate::CreateWeakLambda(this, [PC_Player]()
				{
					if (PC_Player)
					{
						PC_Player->SetWidget(TEXT("DataFragmentPickupWidget"), false, EMessageTargetType::Multicast);
					}
				}),
			WidgetDuration,
			false
		);
	}
}

void ADataFragment::OnRep_bIsActived()
{
	SetDataFragmentPickupWidget();
}

void ADataFragment::HideFragmentMesh()
{
	if (UStaticMeshComponent* Mesh = FindComponentByClass<UStaticMeshComponent>())
	{
		Mesh->SetHiddenInGame(true);
		Mesh->SetVisibility(false);
		Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}