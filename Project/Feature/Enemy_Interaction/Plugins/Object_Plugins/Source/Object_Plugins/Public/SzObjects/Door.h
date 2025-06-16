// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SzObjects/BaseObject.h"
#include "Door.generated.h"

/**
 * 
 */
UCLASS()
class OBJECT_PLUGINS_API ADoor : public ABaseObject
{
	GENERATED_BODY()
	
public:
	ADoor();

protected:
	virtual void BeginPlay() override;

public:
    virtual void OnInteractSever_Implementation(APawn* Interactor) override;
	virtual bool CanInteract_Implementation(const APawn* Interactor) const override;

    // 블루프린트에서 문을 여는 동작을 구현할 수 있도록 선언
    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Door")
    void OpenDoor();
    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Door")
    void CloseDoor();

protected:
    // 연결된 객체들(예: 해킹 컴퓨터)이 모두 활성화되었는지 확인
    bool AreAllObjActived() const;

    // 열쇠 소지 여부 확인
    bool HasKey(const APawn* Interactor) const;

    UFUNCTION(NetMulticast, Reliable)
    void S2A_OpenDoor();

    // 문 여는 횟수 제한(임시 : 지울거임)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
    int32 OpenNum = 0;

protected:
    // 문이 열리기 위해 필요한 Actor 목록 (예: 해킹 트리거)
    UPROPERTY(EditAnywhere, Category = "Door")
    TArray<AActor*> Triggers;

    // 요구하는 열쇠 Actor
    UPROPERTY(EditAnywhere, Category = "Door")
    TObjectPtr<AActor> RequiredKey;

    // 문이 열린 상태
    bool bIsOpened = false;

    // 필요 활성화 수 (0이면 Triggers의 전체 수가 기본값)
    UPROPERTY(EditAnywhere, Category = "Door")
    int32 NeededActive = 0;


    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
    int32 SecurityLevel = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
    bool IsActive = false;
};
