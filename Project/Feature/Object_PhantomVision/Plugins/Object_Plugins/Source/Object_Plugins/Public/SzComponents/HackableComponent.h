// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "HackableComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class OBJECT_PLUGINS_API UHackableComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHackableComponent();

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:	
	UFUNCTION(BlueprintCallable, Category = "Hacking")
	virtual void HackingStarted(APawn* Interactor);

	UFUNCTION(BlueprintCallable, Category = "Hacking")
	virtual void HackingCompleted(APawn* Interactor);

	UFUNCTION(BlueprintCallable, Category = "Hacking")
	virtual void UpdateHackingProgress(APawn* Interactor, float CurrentTime);
	virtual void TryCompleteHacking(APawn* Interactor, float HeldDuration, float CurrentTime);

	// 해킹 성공 후 일정 시간이 지나면 다시 해킹 가능 상태로 초기화
	virtual void CheckHackReset(APawn* Interactor);
	virtual void CheckHackReset();

private:
	APlayerController* GetPlayerControllerFromPawn(APawn* Pawn);

public:
	/* 클라이언트 RPC 함수들 */
	UFUNCTION(Client, Reliable)
	void S2C_ShowHackingUI();	// 해킹 UI 표시 (로컬에서만)
	void S2C_ShowHackingUI_Implementation();

	UFUNCTION(Client, Reliable)
	void S2C_UpdateHackingProgress(float HeldDuration);	// 해킹중 UI 시간 업데이트 (로컬에서만)
	void S2C_UpdateHackingProgress_Implementation(float HeldDuration);

	UFUNCTION(Client, Reliable)
	void S2C_ShowCompletedMessage();	// 해킹 완료 메세지 표시
	void S2C_ShowCompletedMessage_Implementation();

	UFUNCTION(Client, Reliable)
	void S2C_HideHackingUI();	// UI 제거
	void S2C_HideHackingUI_Implementation();

public:
	/* 로컬용 or const(런타임에 수정X) */
	UPROPERTY(EditDefaultsOnly, Category = "Hacking")
    TSubclassOf<class UUserWidget> HackingGaugeWidget;

	UPROPERTY()
	TObjectPtr<class UUserWidget> GuageUI;	// 해킹 게이지 위젯
	
	UPROPERTY(EditAnywhere, Category = "Hacking")
	float HackedDuration = 5.0f;			// 해킹 완료 후 유지 시간

	UPROPERTY(EditAnywhere, Category = "Hacking")
	float RequiredTime = 2.0f;				// 해킹 키 홀딩 시간

	float HackingStartTime = 0.0f;			// 해킹 시작 시간 (GetWorld()->GetTimeSeconds()로 초기화됨)
	bool bAutoHackingCompleted = false;		// 자동 해킹 완료 여부 (true면 HeldTime이 충분하면 자동으로 해킹 성공 처리됨)

	/* 서버용 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hacking", Replicated)
	bool bKeepHacked = false;	// 해킹 유지 여부 (true면 무한 해킹 유지)

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	bool bIsHacking = false;	// 해킹 키를 누르고 있는지
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	bool bIsHacked = false;		// 해킹 상태

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	TObjectPtr<class APlayerController> CurrentHackingPlayer;	// 현재 해킹을 실행 중인 플레이어 컨트롤러 (복제됨)

};
