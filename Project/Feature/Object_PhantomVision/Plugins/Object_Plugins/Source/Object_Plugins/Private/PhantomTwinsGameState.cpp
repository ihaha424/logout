// Fill out your copyright notice in the Description page of Project Settings.


#include "PhantomTwinsGameState.h"
#include "Net/UnrealNetwork.h"

APhantomTwinsGameState::APhantomTwinsGameState()
{
	CCTVManager = CreateDefaultSubobject<UCCTVManager>(TEXT("CCTVManager"));
}

//void APhantomTwinsGameState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
//{
//	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
//
//	DOREPLIFETIME(APhantomTwinsGameState, CCTVManager);
//
//	/*
//		CreateDefaultSubobject로 생성한 컴포넌트는 별도의 Replicated UPROPERTY가 필요 없고,
//		GetLifetimeReplicatedProps에서 등록할 필요가 없다고 안내합니다.
//	*/
//}
