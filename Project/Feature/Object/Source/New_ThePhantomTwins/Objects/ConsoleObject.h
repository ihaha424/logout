// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SzObjects/InteractableObject.h"
#include "ConsoleObject.generated.h"

class APS_Player;

UCLASS()
class NEW_THEPHANTOMTWINS_API AConsoleObject : public AInteractableObject
{
	GENERATED_BODY()
	
public:	
	AConsoleObject();

protected:
	virtual void BeginPlay() override;

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual bool CanInteract_Implementation(const APawn* Interactor, bool bIsDetected) override;
	virtual void OnInteractServer_Implementation(const APawn* Interactor) override;
	virtual void OnInteractClient_Implementation(const APawn* Interactor) override;

	virtual void SetWidgetVisible(bool bVisible) override;

protected:
	// LevelDataFragments 개수와 현재 수집 개수를 비교하여 bIsCollectionCompleted 를 설정
	UFUNCTION()
	void UpdateCollectionCompletionState();

	// 모든 클라이언트에 위젯 보이기/숨기기 (서버 호출 -> 멀티캐스트로 전파)
    UFUNCTION(NetMulticast, Reliable)
	void S2A_ShowWaitingPlayerWidget(bool bVisible);
	void S2A_ShowWaitingPlayerWidget_Implementation(bool bVisible);

	// 안전한 타이머 콜백: PlayerState 기반으로 엔딩 검사
	UFUNCTION()
	void CheckEndingConditionByPlayerState(APS_Player* InteractorPlayerState);

	// 멀티캐스트: 서버가 호출 -> 모든 클라이언트에서 실행, 각 클라이언트는 Interactor가 자신인지 검사
	UFUNCTION(NetMulticast, Reliable)
	void S2A_InvokePlaySoloEnding(APawn* Interactor);
	void S2A_InvokePlaySoloEnding_Implementation(APawn* Interactor);

	// 2인 탈출 : 진엔딩 실행(BlueprintImplementableEvent)
	UFUNCTION(BlueprintImplementableEvent, Category = "Wait5Seconds")
	void PlayTrueEnding();

	// 1인 탈출 : 1인 엔딩 실행(BlueprintImplementableEvent)
	UFUNCTION(BlueprintImplementableEvent, Category = "SoloLogOut")
	void PlaySoloEnding(const APawn* Interactor);

	// 솔로 포탈 콜리젼 키기 함수
	UFUNCTION(BlueprintImplementableEvent, Category = "SoloLogOut")
	void SetSoloPortalCollisionFlag(bool bActived);

	// 상태 리셋 함수 추가
    UFUNCTION(NetMulticast, Reliable)
    void S2A_ResetConsoleState();
    void S2A_ResetConsoleState_Implementation();

private:
	void ShowAndAutoRemoveWaitWidgets(class APC_Player* PC_Player);

	// 맵 전체에서 LogOutReady 태그를 가진 플레이어 수를 계산하는 헬퍼
	int32 CountPlayersWithLogOutReadyTag() const;

	// 모든 플레이어의 LogOutReady 태그 제거
	void ClearAllLogOutReadyTags();

	int32 CheckLevelPlayers();

protected:
	// 현재 레벨에 존재하는 LevelDataFragments
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Replicated)
	TArray<TObjectPtr<class ADataFragment>> LevelDataFragments;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "ConsoleObject", Replicated)
	bool bIsCollectionCompleted = false;

	// 데이터조각 수집 전, 콘솔 잠금
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ConsoleObject | ObjectWidget")
	TObjectPtr<class UWidgetComponent> LockWidgetComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ConsoleObject | ObjectWidget")
	TSubclassOf<class UUserWidget> LockWidgetClass;

	// 한명이라도 콘솔과 상호작용하면, 콘솔 위에 뜨는 위젯
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ConsoleObject | Widget")
	TObjectPtr<class UWidgetComponent> WaitingPlayerWidgetComp;

	// 상호작용 후, 5초 기다리는 타이머
	FTimerHandle Wait5SecTimerHandle;
};
