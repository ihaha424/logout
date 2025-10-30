// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SzObjects/InteractableObject.h"
#include "SzInterface/Destroyable.h"
#include "Door.generated.h"

/**
 * 
 */
UCLASS()
class NEW_THEPHANTOMTWINS_API ADoor : public AInteractableObject, public IDestroyable
{
	GENERATED_BODY()

public:
	ADoor();

protected:
	virtual void BeginPlay() override;

	virtual void SetWidgetVisible(bool bVisible) override;

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual bool CanInteract_Implementation(const APawn* Interactor, bool bIsDetected) override;
	virtual void OnInteractServer_Implementation(const APawn* Interactor) override;
	virtual void OnInteractClient_Implementation(const APawn* Interactor) override;

	virtual bool CanBeDestroyed_Implementation(const APawn* Interactor) override;
	
	UFUNCTION(BlueprintCallable, Category = "Door")
	void CheckAndUpdateDoorState();

    // trigger들이 모두 활성화되었는지 확인 (AInteractableObject의 bActived를 사용하는게 아니라면 override해서 작성)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Door")
	bool AreAllTriggerActived();

	virtual void OnRep_bIsActived() override;
protected:
	bool AreAllTriggerActived_Implementation();



    UFUNCTION(NetMulticast, Reliable)
    void S2A_OpenDoor();

	UFUNCTION(NetMulticast, Reliable)
    void S2A_CloseDoor();

    // 블루프린트에서 문을 여는 동작을 구현할 수 있도록 선언
    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Door")
	void OpenDoor();

	// 블루프린트에서 문을 닫는 동작을 구현할 수 있도록 선언
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Door")
    void CloseDoor();

	// 블루프린트에서 잠긴 문과 상호작용할 때, 구현할 수 있도록 선언
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Door")
	void ShowLockedDoorDialog(const APawn* Interactor);


public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InteractableObject | ObjectWidget")
	TObjectPtr<class UWidgetComponent> LockWidgetComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InteractableObject | ObjectWidget")
	TSubclassOf<class UUserWidget> LockWidgetClass;

	// 문이 열리기 위해 필요한 Actor 목록
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	TArray<AActor*> RequiredList;

	// 필요 활성화 수 (0이면 RequiredList의 전체 수가 기본값)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
    int32 MinRequiredCount = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Door", ReplicatedUsing = OnRep_bKeyUsed)  
	bool bKeyUsed = false;
	UPROPERTY(Replicated, BlueprintReadWrite)
	bool bIsAllTriggered = false;

	UFUNCTION()
	virtual void OnRep_bKeyUsed();

	UFUNCTION(BlueprintImplementableEvent)
	void CheckAllTriggered();
};
// Door 클래스 함수 중 블프에서 수정할 수 있는 함수 : AreAllTriggerActived(), OpenDoor(), CloseDoor()