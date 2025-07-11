// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SzObjects/InteractableObject.h"
#include "Door.generated.h"

/**
 * 
 */
UCLASS()
class NEW_THEPHANTOMTWINS_API ADoor : public AInteractableObject
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

protected:
    // trigger들이 모두 활성화되었는지 확인 (AInteractableObject의 bActived를 사용하는게 아니라면 override해서 작성)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ADoor")
	bool AreAllTriggerActived() const;
	bool AreAllTriggerActived_Implementation() const;


    UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
    void S2A_OpenDoor();
    void S2A_OpenDoor_Implementation();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InteractableObject | ObjectWidget")
	TObjectPtr<class UWidgetComponent> LockWidgetComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InteractableObject | ObjectWidget")
	TSubclassOf<class UUserWidget> LockWidgetClass;

	// 문이 열리기 위해 필요한 Actor 목록 (제거)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	TArray<AActor*> RequiredList;

	// 필요 활성화 수 (0이면 RequiredList의 전체 수가 기본값)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
    int32 MinRequiredCount = 0;
};
