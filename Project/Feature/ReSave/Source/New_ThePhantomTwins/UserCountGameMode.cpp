// Fill out your copyright notice in the Description page of Project Settings.


#include "UserCountGameMode.h"
#include "Engine/GameInstance.h"
#include "LevelTravelSystem/TravelManagerSubsystem.h"

void AUserCountGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	GetGameInstance()->GetSubsystem<UTravelManagerSubsystem>()->PostLogin(NewPlayer);
}

void AUserCountGameMode::Logout(AController* Exiting)
{
	// 1. Null가드
	if (!Exiting)
	{
		Super::Logout(Exiting);
		return;
	}

	// 2. 필요한 정보 캐싱 (PlayerState 등)

	// 3. Super 호출 (PlayerState 제거 등 내부 정리)
	Super::Logout(Exiting);


	// 4. GameInstance/SubSystem 널 가드 후 접근
	UGameInstance* GI = GetGameInstance();
	if (!IsValid(GI))
		return;

	UTravelManagerSubsystem* Sub = GI->GetSubsystem<UTravelManagerSubsystem>();
	if (IsValid(Sub))
	{
		Sub->Logout(Exiting);
	}
}

/*

	순서										의미													위험
	Super::Logout() 이전에 Subsystem 접근	아직 월드, GameInstance, Subsystem 다 살아 있음		안전하지만, PlayerState가 남아 있어서 “아직 완전히 로그아웃 안 된 상태”일 수도 있음
	Super::Logout() 이후에 Subsystem 접근	PlayerState 제거 완료, 네트워크 정리 진행 중			GameInstance는 살아있지만 PlayerController/PlayerState는 무효. 안전하게 Null 체크 필요

*/