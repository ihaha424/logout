#include "DataFragment.h"
#include "Components/StaticMeshComponent.h"
#include "TimerManager.h"
#include "Blueprint/UserWidget.h"
#include "../Player/PC_Player.h"
#include "../Player/PlayerCharacter.h"
#include "../UI/DataFragmentPickupWidget.h"
#include "GS_PhantomTwins.h"
#include "Kismet/GameplayStatics.h"
#include "Log/TPTLog.h"
#include "SaveGame/SaveIDComponent.h"
#include "SaveGame/TPTSaveGameManager.h"

ADataFragment::ADataFragment()
{
	RootLocation = CreateDefaultSubobject<USceneComponent>(TEXT("RootLocation"));
	RootLocation->SetupAttachment(RootSceneComp);

	HostPlayerLocation = CreateDefaultSubobject<USceneComponent>(TEXT("HostPlayerLocation"));
	HostPlayerLocation->SetupAttachment(RootLocation);

	ClientPlayerLocation = CreateDefaultSubobject<USceneComponent>(TEXT("ClientPlayerLocation"));
	ClientPlayerLocation->SetupAttachment(RootLocation);

	PlayersLocation.Add(HostPlayerLocation);
	PlayersLocation.Add(ClientPlayerLocation);
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

	const APlayerCharacter* Player = Cast<APlayerCharacter>(Interactor);
	NULLCHECK_RETURN_LOG(Player, ItemLog, Warning, );

	UTPTSaveGameManager* SaveGameManager = GetGameInstance()->GetSubsystem<UTPTSaveGameManager>();
	TPT_LOG(SaveGameLog, Warning, TEXT("11DataFragment : %s"), *GetFName().ToString())
	SaveGameManager->TempSaveByID(GetFName(), false);
	SaveGameManager->SetRestartPoint(this);

	UWorld* World = GetWorld();
	NULLCHECK_RETURN_LOG(World, ItemLog, Warning, );

	if (HasAuthority())
	{
		for (FConstPlayerControllerIterator It = World->GetPlayerControllerIterator(); It; ++It)
		{
			if (APlayerController* PC = It->Get())
			{
				SaveGameManager->TempSavePlayer(PC);
			}
		}
	}
	
	InvokeGameplayCue(Interactor);
	ApplyEffectToTarget(Interactor);

	SetDataFragmentPickupWidget();

	DestroyItem();

	SaveGameManager->SaveUpdate();
}

void ADataFragment::DestroyItem()
{
	HideFragmentMesh();
	SetActorEnableCollision(false);
	SaveToGameState();
	Super::DestroyItem();
}

void ADataFragment::SaveToGameState()
{
	AGS_PhantomTwins* GS = GetWorld()->GetGameState<AGS_PhantomTwins>();
	if (GS)
	{
		GS->AddCollectedItem(this);
	}
}

void ADataFragment::SetDataFragmentPickupWidget()
{

	if (APC_Player* PC_Player = Cast<APC_Player>(GetWorld()->GetFirstPlayerController()))
	{
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
	}
}

void ADataFragment::OnRep_bIsActived()
{
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