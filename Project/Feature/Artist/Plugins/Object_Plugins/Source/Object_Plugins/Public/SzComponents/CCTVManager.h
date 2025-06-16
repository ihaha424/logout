// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SzObjects/CCTV.h"
#include "CCTVManager.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class OBJECT_PLUGINS_API UCCTVManager : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCCTVManager();

protected:
	virtual void BeginPlay() override;

public:
	// CCTV
	void AddCCTV(int32 CCTVId, ACCTV* CCTV);
	void RemoveCCTV(int32 CCTVId);

	ACCTV* GetCCTV(int32 CCTVId) const
	{
		if (const TObjectPtr<ACCTV>* CCTVPtr = CCTVMap.Find(CCTVId))
		{
			return CCTVPtr->Get();
		}
		return nullptr;
	}


	// 해킹된 CCTV
	void AddHackedCCTV(int32 CCTVId);
	void RemoveHackedCCTV(int32 CCTVId);
	bool HasHackedCCTV() const;								// 해킹된 CCTV가 있는지 확인하는 함수
	ACCTV* GetFirstHackedCCTV() const;						// HackedIDSet 을 sorting 했을 때 가장 첫번째 CCTV를 반환
	ACCTV* GetPrevHackedCCTV(int32 CurrentCCTVId) const;	// 현재 CCTV의 이전 CCTV를 반환해주는 함수
	ACCTV* GetNextHackedCCTV(int32 CurrentCCTVId) const;	// 현재 CCTV의 이후 CCTV를 반환해주는 함수	
	
	ACCTV* GetHackedCCTV(int32 CCTVId) const
	{
		if (HackedIDSet.Contains(CCTVId))
		{
			if (const TObjectPtr<ACCTV>* CCTVPtr = CCTVMap.Find(CCTVId))
			{
				return CCTVPtr->Get();
			}
		}
		return nullptr;
	}


		
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CCTV")
	TMap<int32, TObjectPtr<ACCTV>> CCTVMap;				// 전체 CCTV들

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CCTV")
	TSet<int32> HackedIDSet;		// 해킹된 CCTV id 모음

};
