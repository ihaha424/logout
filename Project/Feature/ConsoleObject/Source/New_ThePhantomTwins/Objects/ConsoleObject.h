// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SzObjects/InteractableObject.h"
#include "ConsoleObject.generated.h"

/**
 * 
 */
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

	virtual void SetWidgetVisible(bool bVisible) override;

	UFUNCTION()
    void OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);

    UFUNCTION()
    void OnTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	bool AreAllTriggerActived() const;

	virtual void OnRep_bIsActived() override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ConsoleObject | ObjectWidget")
	TObjectPtr<class UWidgetComponent> LockWidgetComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ConsoleObject | ObjectWidget")
	TSubclassOf<class UUserWidget> LockWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ConsoleObject | Hide")
	TObjectPtr<class USphereComponent> Trigger;

	UPROPERTY(Replicated)
	int32 HasPlayerNum = 0;

	UPROPERTY(EditAnywhere, Category = "ConsoleObject")
	int32 MaxPlayerNum = 2;

	// 문이 열리기 위해 필요한 Actor 목록
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ConsoleObject")
	TArray<AActor*> RequiredList;

	// 필요 활성화 수 (0이면 RequiredList의 전체 수가 기본값)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ConsoleObject")
    int32 MinRequiredCount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ConsoleObject | Door")
	TObjectPtr<class ADoor> ConnectedDoor;

};
