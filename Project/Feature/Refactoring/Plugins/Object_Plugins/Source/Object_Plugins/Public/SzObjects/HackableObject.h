// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SzObjects/BaseObject.h"
#include "SzInterface/Hacking.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "HackableObject.generated.h"

UCLASS()
class OBJECT_PLUGINS_API AHackableObject : public ABaseObject, public IHacking
{
	GENERATED_BODY()
	
public:	
	AHackableObject();

protected:
    virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:
	// 해킹 실행 (E키 홀딩)
	virtual void OnHackingStartedServer_Implementation(const APawn* Interactor) override;

	// 해킹 완료 후 로직 (CCTV 보임, 적 무력화 등)
	virtual void OnHackingCompletedServer_Implementation(const APawn* Interactor) override;
    
	// 해킹 가능 여부 체크 (false => 해킹 전 / true => 해킹 완료)
	virtual bool CanBeHacked_Implementation(const APawn* Interactor) override;

	// 해킹 초기화(해킹 안 된 상태로 만들기)
	virtual void ClearHacking_Implementation(const APawn* Interactor) override;



protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hacking")
	TObjectPtr<class UHackableComponent> HackingComp;


private:
	UPROPERTY()
	TObjectPtr<const APawn> CurrentHackingPawn;	// 현재 해킹 중인 플레이어를 추적하기 위한 변수
};
