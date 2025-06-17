// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SzObjects/CCTV.h"
#include "CCTVManager.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogCameraManger, Log, All);


USTRUCT(BlueprintType)
struct FCameraInfomation
{
	GENERATED_BODY()
public:
	FCameraInfomation() 
		: CCTV(nullptr)
		, IsHacking(false)
	{}

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CCTV")
	TObjectPtr<ACCTV> CCTV;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CCTV")
	bool IsHacking;
};




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
	void	AddCCTV(int32 CCTVId, ACCTV* CCTV);
	ACCTV*	GetCCTV(int32 CCTVId) const;
	void	SetHackedCCTV(int32 CCTVId, bool IsHacking);
	/**
	 * @brief 
			: 해킹된 CCTV가 있는지 확인하는 함수
	 * @return 
			: Hacked CCTV Count 
	 */
	int32	HasHackedCCTV() const;
	ACCTV* GetFirstHackedCCTV() const;						// HackedIDSet 을 sorting 했을 때 가장 첫번째 CCTV를 반환
	ACCTV* GetPrevHackedCCTV(int32 CurrentCCTVId) const;	// 현재 CCTV의 이전 CCTV를 반환해주는 함수
	ACCTV* GetNextHackedCCTV(int32 CurrentCCTVId) const;	// 현재 CCTV의 이후 CCTV를 반환해주는 함수	
	ACCTV* GetHackedCCTV(int32 CCTVId) const;


		
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "CCTV")
	TArray<FCameraInfomation> CCTVList;



private:
	const int32 InitializeCameraMaxSize = 15;

};
