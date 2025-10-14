#include "DataFragment.h"
#include "Components/StaticMeshComponent.h"
#include "TimerManager.h"
#include "Blueprint/UserWidget.h"
#include "../Player/PC_Player.h"
#include "../Player/PlayerCharacter.h"
#include "../UI/HUD/PlayerHUDWidget.h"
#include "../UI/DataFragmentPickupWidget.h"
#include "GS_PhantomTwins.h"
#include "Log/TPTLog.h"
#include "SaveGame/TPTSaveGame.h"
#include "SaveGame/TPTSaveGameHelperLibrary.h"

ADataFragment::ADataFragment()
{
}

void ADataFragment::BeginPlay()
{
	Super::BeginPlay();

	if (APC_Player* PC_Player = Cast<APC_Player>(GetWorld()->GetFirstPlayerController()))
	{

		if (!DataFragmentPickupWidgetClass)
		{
			UE_LOG(LogTemp, Warning, TEXT("DataFragmentPickupWidgetClass is null! Cannot create widget."));
			return;
		}

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

	DestroyItem();
}

void ADataFragment::SetDataFragmentPickupWidget()
{
	HideFragmentMesh();
	SetActorEnableCollision(false);

	if (APC_Player* PC_Player = Cast<APC_Player>(GetWorld()->GetFirstPlayerController()))
	{
		AGS_PhantomTwins* GS = GetWorld()->GetGameState<AGS_PhantomTwins>();
		if (GS)
		{
			GS->AddCollectedItem(this);
		}
		
		// 팝업 위젯
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

		APlayerCharacter* MyCharacter = Cast<APlayerCharacter>(PC_Player->GetPawn());
		NULLCHECK_RETURN_LOG(MyCharacter, ItemLog, Warning, );
		UTPTSaveGame* TPTSaveGame = UTPTSaveGameHelperLibrary::GetSaveGameData<UTPTSaveGame>();
		TPTSaveGame->PlayerLocation = MyCharacter->GetActorLocation();
		TPTSaveGame->PlayerRotation = MyCharacter->GetActorRotation();
		UTPTSaveGameHelperLibrary::SetSaveGameData<UTPTSaveGame>(TPTSaveGame);
	}
}

void ADataFragment::OnRep_bIsActived()
{
	SetDataFragmentPickupWidget();

	DestroyItem();
}

void ADataFragment::HideFragmentMesh()
{
	TArray<UStaticMeshComponent*> Meshes;
	GetComponents<UStaticMeshComponent>(Meshes);

	for (UStaticMeshComponent* Mesh : Meshes)
	{
		if (Mesh)
		{
			Mesh->SetHiddenInGame(true);
			Mesh->SetVisibility(false);
			Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}
}