// Fill out your copyright notice in the Description page of Project Settings.

#include "EndingConsole.h"
#include "Net/UnrealNetwork.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Blueprint/UserWidget.h"
#include "player/PlayerCharacter.h"
#include "player/PC_Player.h"
#include "player/PS_Player.h"
#include "Log/TPTLog.h"
#include "Components/DecalComponent.h"
#include "Objects/DataFragment.h"
#include "Kismet/GameplayStatics.h"
#include "GS_PhantomTwins.h"
#include "AbilitySystemComponent.h"
#include "Tags/TPTGameplayTags.h"
#include "Kismet/KismetSystemLibrary.h"

AEndingConsole::AEndingConsole() : AInteractableObject()
{
	bReplicates = true;

	// Lock Widget
	LockWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("LockWidget"));
	LockWidgetComp->SetupAttachment(RootComponent);
	LockWidgetComp->SetWidgetSpace(EWidgetSpace::Screen);
	LockWidgetComp->SetDrawSize(FVector2D(10, 10));
	LockWidgetComp->SetRelativeLocation(FVector(0, 0, 100));

	// Console 위에 뜨는 3D 위젯
	WaitingPlayer3DWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("WaitingPlayerWidget"));
	WaitingPlayer3DWidgetComp->SetupAttachment(RootComponent);
	WaitingPlayer3DWidgetComp->SetWidgetSpace(EWidgetSpace::World);
}

void AEndingConsole::BeginPlay()
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

	if (WaitingPlayer3DWidgetComp)
	{
		WaitingPlayer3DWidgetComp->SetVisibility(false);
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

void AEndingConsole::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(Wait5SecTimerHandle);
		GetWorld()->GetTimerManager().ClearTimer(RemoveWidgetTimerHandle);
		RemoveWidgetDelegate = nullptr;
	}

	Super::EndPlay(EndPlayReason);
}

void AEndingConsole::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AEndingConsole, LevelDataFragments);
	DOREPLIFETIME(AEndingConsole, bIsCollectionCompleted);
	DOREPLIFETIME(AEndingConsole, AllowedInteractor);
	DOREPLIFETIME(AEndingConsole, FirstInteractPC);
}

bool AEndingConsole::CanInteract_Implementation(const APawn* Interactor, bool bIsDetected)
{
	if (!Interactor->IsLocallyControlled()) return bIsDetected;

	// 거리가 멀어져 감지되지 않은 경우
	if (!bIsDetected)
	{
		SetWidgetVisible(false);
		return false;
	}

	UpdateCollectionCompletionState();

	// Interactor가 APlayerCharacter 인 경우에만 위젯을 띄어라
	const APlayerCharacter* PlayerChar = Cast<APlayerCharacter>(const_cast<APawn*>(Interactor));
	if (PlayerChar)
	{
		SetWidgetVisible(true);
	}

	return true;
}

void AEndingConsole::OnInteractServer_Implementation(const APawn* Interactor)
{
	//UKismetSystemLibrary::PrintString(this, TEXT("AConsoleObject :: OnInteractServer"));

	UpdateCollectionCompletionState();

	if (!bIsCollectionCompleted) return;

	APlayerCharacter* PlayerChar = Cast<APlayerCharacter>(const_cast<APawn*>(Interactor));
	NULLCHECK_RETURN_LOG(PlayerChar, ObjectLog, Error, );
	APlayerController* InteractorPC = Cast<APlayerController>(PlayerChar->GetController());
	NULLCHECK_RETURN_LOG(InteractorPC, ObjectLog, Error, );
	APS_Player* PS = InteractorPC->GetPlayerState<APS_Player>();
	NULLCHECK_RETURN_LOG(PS, ObjectLog, Error, );
	UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
	NULLCHECK_RETURN_LOG(ASC, ObjectLog, Error, );

	// 플레이어에 LogOutReady 태그가 없다면 붙임
	if (!ASC->HasMatchingGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_State_LogOutReady))
	{
		ASC->AddLooseGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_State_LogOutReady);
	}

	// 모든 플레이어에게 3D 위젯 보이기
	S2A_ShowWaitingPlayerWidget(true);

	// --- 변경된 검사: 맵 전체에서 LogOutReady 태그를 가진 플레이어 수를 센다
	int32 NumReady = CountPlayersWithLogOutReadyTag();

	if (NumReady >= CheckLevelPlayers())
	{
		// 이미 다른 플레이어(혹은 적어도 2명 이상)가 상호작용 상태라면 즉시 진엔딩 실행
		// 위젯 숨기고 타이머 정리
		S2A_ShowWaitingPlayerWidget(false);
		GetWorld()->GetTimerManager().ClearTimer(Wait5SecTimerHandle);
		GetWorld()->GetTimerManager().ClearTimer(RemoveWidgetTimerHandle);
		RemoveWidgetDelegate = nullptr;
		S2A_HiddenWaitWidgetsFromAllPlayers();

		PlayTrueEnding();
		FirstInteractPC = nullptr;
	}
	else
	{
		// 아무도 상호작용하지 않았다면(첫 상호작용자라면)
		// 서버에서 5초 후 엔딩 체크 (PlayerState 기반으로 안전하게 처리)
		FirstInteractPC = Cast<APC_Player>(InteractorPC);
		GetWorld()->GetTimerManager().ClearTimer(Wait5SecTimerHandle);
		GetWorld()->GetTimerManager().ClearTimer(RemoveWidgetTimerHandle);
		RemoveWidgetDelegate = nullptr;

		FTimerDelegate TimerDel;
		// PlayerState (PS)를 저장하여 타이머 콜백에서 사용 — Pawn이 사라져도 PlayerState는 보통 유지됨
		TimerDel.BindUObject(this, &AEndingConsole::CheckEndingConditionByPlayerState, PS);

		GetWorld()->GetTimerManager().SetTimer(
			Wait5SecTimerHandle,
			TimerDel,
			5.0f,
			false
		);
	}
}


void AEndingConsole::OnInteractClient_Implementation(const APawn* Interactor)
{
	//UKismetSystemLibrary::PrintString(this, TEXT("AConsoleObject :: OnInteractClient"));

	SetWidgetVisible(false);

	if (!bIsCollectionCompleted) return;

	APC_Player* PC_Player = Cast<APC_Player>(Interactor->GetController());
	NULLCHECK_RETURN_LOG(PC_Player, ObjectLog, Error, );

	// 팝업 위젯 (5초 뒤 자동 제거)
	if (!bIsActived)
	{
		ShowAndAutoRemoveWaitWidgets(PC_Player);
	}

	bIsActived = true;

}

void AEndingConsole::SetWidgetVisible(bool bVisible)
{
	if (!InteractWidgetComp) return;

	// 아웃라인 코드
	if (bIsActived)
	{
		ShowOverlayOutline(!bIsActived);
	}

	if (!bVisible)
	{
		// 감지 안 되었거나 명시적으로 숨길 때는 둘 다 숨김
		InteractWidgetComp->SetVisibility(false);
		LockWidgetComp->SetVisibility(false);
		return;
	}

	// 위젯 가시성 결정:
	if (!bIsCollectionCompleted)
	{
		// 잠금 위젯 보여주고 상호작용 위젯 숨김
		InteractWidgetComp->SetVisibility(false);
		LockWidgetComp->SetVisibility(true);
	}
	else
	{
		// 상호작용 가능: 상호작용 위젯 보여주고 잠금 위젯 숨김
		InteractWidgetComp->SetVisibility(true);
		LockWidgetComp->SetVisibility(false);
	}
}

void AEndingConsole::UpdateCollectionCompletionState()
{
	// GS에서 OnCollectedItemCountChanged() 가져옴
	AGS_PhantomTwins* GS = Cast<AGS_PhantomTwins>(GetWorld()->GetGameState());
	int32 currDataFragments = 0;
	if (GS)
	{
		currDataFragments = GS->DataFragmentCount;
	}

	// currDataFragments가 LevelDataFragments보다 작으면 bIsCollectionCompleted = false;
	// 같거나 크면 true
	bool bNewCollectionCompleted = (currDataFragments >= LevelDataFragments.Num());
	bIsCollectionCompleted = bNewCollectionCompleted;
}

void AEndingConsole::S2A_ShowWaitingPlayerWidget_Implementation(bool bVisible)
{
	// 모든 플레이어한테 3D UI WBP_WaitingPlayer 띄움
	if (WaitingPlayer3DWidgetComp)
	{
		WaitingPlayer3DWidgetComp->SetVisibility(bVisible);
	}
}

void AEndingConsole::CheckEndingConditionByPlayerState(APS_Player* InteractorPlayerState)
{
	//UKismetSystemLibrary::PrintString(this, TEXT("CheckEndingConditionByPlayerState called (server)"));

	// 서버에서만 실행되어야 함
	if (!HasAuthority()) return;

	// WaitingPlayerWidget 숨기기
	S2A_ShowWaitingPlayerWidget(false);

	// 맵 전체에서 LogOutReady 태그 가진 플레이어 수 확인
	int32 NumReady = CountPlayersWithLogOutReadyTag();

	if (NumReady >= CheckLevelPlayers())
	{
		// 모두가 준비된 상태라면 위젯 숨기기
		S2A_HiddenWaitWidgetsFromAllPlayers();

		// 둘 이상이면 진엔딩
		PlayTrueEnding();
	}
	else
	{
		// 아니면 솔로 엔딩
		APawn* InteractorPawn = nullptr;
		if (InteractorPlayerState)
		{
			InteractorPawn = InteractorPlayerState->GetPawn();
		}

		// 멀티캐스트로 모든 클라이언트에게 알리고, 각 클라이언트는 자기인지 검사 후 로컬 엔딩 실행
		S2A_InvokePlaySoloEnding(InteractorPawn);

		// TODO: Collision 키기 그냥 함수
		SetSoloPortalCollisionFlag(true);

		// TODO: 현재 자신이 clinet에 의한건지 host에 의한건지 bool? 로 파악
		NotifySoloPortalOfInteractor(InteractorPawn);
		S2A_NotifySoloPortalOfInteractor(InteractorPawn);

		// 솔로 엔딩 후 상태 리셋 (다시 상호작용 가능하도록)
		S2A_ResetConsoleState();
	}

	// 타이머는 이미 콜백이므로 필요시 정리 (안전장치)
	GetWorld()->GetTimerManager().ClearTimer(Wait5SecTimerHandle);
	GetWorld()->GetTimerManager().ClearTimer(RemoveWidgetTimerHandle);
	RemoveWidgetDelegate = nullptr;
}

void AEndingConsole::S2A_InvokePlaySoloEnding_Implementation(APawn* Interactor)
{
	if (!Interactor)
		return;

	// 서버에서는 항상 실행
	if (HasAuthority())
	{
		if (Interactor->IsLocallyControlled())
		{
			// TODO: 비쥬얼만 콜리션 X
			PlaySoloEnding(Interactor);
		}
	}
	else
	{
		// 클라이언트에서는 자기 Pawn인지 확인 후 실행
		if (Interactor->IsLocallyControlled())
		{
			PlaySoloEnding(Interactor);
		}
	}

}

void AEndingConsole::S2A_ResetConsoleState_Implementation()
{
	// 모든 클라이언트에서 실행됨

	// bIsActived를 false로 리셋
	bIsActived = false;
	FirstInteractPC = nullptr;

	// 서버에서만 태그 제거 로직 실행
	if (HasAuthority())
	{
		ClearAllLogOutReadyTags();
	}

	// 위젯 상태 초기화
	if (WaitingPlayer3DWidgetComp)
	{
		WaitingPlayer3DWidgetComp->SetVisibility(false);
	}
}

void AEndingConsole::ShowAndAutoRemoveWaitWidgets(class APC_Player* PC_Player)
{
	if (!PC_Player) return;

	if (!PC_Player->GetWidget(TEXT("CloseWait5Sec"))) return;
	//if (!PC_Player->GetWidget(TEXT("FarWait5Sec"))) return;


	// 게임 안의 플레이어가 2명이상인지 체크.
	// 1명이면 return

	// 방법 A: GameState의 PlayerArray 기준으로 카운트
	AGameStateBase* GSBase = GetWorld()->GetGameState();
	int32 NumPlayers = 0;
	if (GSBase)
	{
		// PlayerArray에 들어있는 플레이어수 기준으로 계산
		NumPlayers = GSBase->PlayerArray.Num();
	}
	else
	{
		UE_LOG(ObjectLog, Warning, TEXT("ShowAndAutoRemoveWaitWidgets: GameState is null; falling back to PlayerController count"));
		// 방법 B: PlayerController 수로 카운트
		NumPlayers = GetWorld()->GetNumPlayerControllers();
	}

	if (NumPlayers < 2)
	{
		UE_LOG(ObjectLog, Log, TEXT("ShowAndAutoRemoveWaitWidgets: Not enough players (%d) - skipping widget"), NumPlayers);
		return;
	}

	// 위젯 표시
	PC_Player->SetWidget(TEXT("CloseWait5Sec"), true, EMessageTargetType::LocalClient);
	PC_Player->SetWidget(TEXT("FarWait5Sec"), true, EMessageTargetType::AllExceptSelf);

	// 5초 뒤 위젯 제거 (타이머 등록)
	RemoveWidgetDelegate.BindLambda([PC_Player]()
		{
			if (PC_Player)
			{
				if (!PC_Player->GetWidget(TEXT("CloseWait5Sec"))) return;

				PC_Player->SetWidget(TEXT("CloseWait5Sec"), false, EMessageTargetType::LocalClient);
				PC_Player->SetWidget(TEXT("FarWait5Sec"), false, EMessageTargetType::AllExceptSelf);
			}
		});

	PC_Player->GetWorldTimerManager().SetTimer(
		RemoveWidgetTimerHandle,
		RemoveWidgetDelegate,
		5.0f,
		false
	);
}

int32 AEndingConsole::CountPlayersWithLogOutReadyTag() const
{
	int32 Count = 0;

	UWorld* World = GetWorld();
	if (!World) return 0;

	AGameStateBase* GSBase = World->GetGameState();
	if (!GSBase) return 0;

	for (APlayerState* PSBase : GSBase->PlayerArray)
	{
		if (!PSBase) continue;

		APS_Player* PS = Cast<APS_Player>(PSBase);
		if (!PS) continue;

		UAbilitySystemComponent* OtherASC = PS->GetAbilitySystemComponent();
		if (!OtherASC) continue;

		if (OtherASC->HasMatchingGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_State_LogOutReady))
		{
			++Count;
		}
	}

	return Count;
}

void AEndingConsole::ClearAllLogOutReadyTags()
{
	UWorld* World = GetWorld();
	if (!World) return;

	AGameStateBase* GSBase = World->GetGameState();
	if (!GSBase) return;

	for (APlayerState* PSBase : GSBase->PlayerArray)
	{
		if (!PSBase) continue;

		APS_Player* PS = Cast<APS_Player>(PSBase);
		if (!PS) continue;

		UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
		if (!ASC) continue;

		// LogOutReady 태그가 있으면 제거
		if (ASC->HasMatchingGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_State_LogOutReady))
		{
			ASC->RemoveLooseGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_State_LogOutReady);
		}
	}
}

int32 AEndingConsole::CheckLevelPlayers()
{
	//	레벨에 존재하는 플레이어들 찾아서 배열에 추가(중복된 플레이어면 넘어감)
	int32 InteractPlayers = 0;
	TArray<AActor*> FoundPlayers;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerCharacter::StaticClass(), FoundPlayers);

	for (AActor* Actor : FoundPlayers)
	{
		APlayerCharacter* PlayerChar = Cast<APlayerCharacter>(Actor);
		if (PlayerChar)
		{
			InteractPlayers++;
		}
	}

	return InteractPlayers;
}

void AEndingConsole::OnRep_AllowedInteractor()
{

}

void AEndingConsole::S2A_NotifySoloPortalOfInteractor_Implementation(APawn* Interactor)
{
	NotifySoloPortalOfInteractor(Interactor);
}

void AEndingConsole::S2A_HiddenWaitWidgetsFromAllPlayers_Implementation()
{
	if (!FirstInteractPC) return;

	// CloseWait5Sec이 존재하면 안보이게 함
	FirstInteractPC->SetWidget(TEXT("CloseWait5Sec"), false, EMessageTargetType::Multicast);
	FirstInteractPC->SetWidget(TEXT("FarWait5Sec"), false, EMessageTargetType::Multicast);
}
