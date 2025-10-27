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
	virtual void OnInteractClient_Implementation(const APawn* Interactor) override;

	virtual void SetWidgetVisible(bool bVisible) override;

	UFUNCTION()
    void OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);

    UFUNCTION()
    void OnTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	virtual void OnRep_bIsActived() override;

protected:
    UFUNCTION(NetMulticast, Reliable)
	void S2A_ShowWaitingPlayerWidget(bool bVisible);
	void S2A_ShowWaitingPlayerWidget_Implementation(bool bVisible);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ConsoleObject | Hide")
	TObjectPtr<class UBoxComponent> SafeZoneTrigger;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ConsoleObject | Door")
	TObjectPtr<class ADoor> ConnectedDoor;

	// 현재 레벨에 존재하는 LevelDataFragments
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Replicated)
	TArray<TObjectPtr<class ADataFragment>> LevelDataFragments;

	// 트리거 안에 있는 플레이어 추적	(추가, 삭제는 Overlap될때 함)
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Replicated)
	TArray<TObjectPtr<class APlayerCharacter>> InteractPlayers;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ConsoleObject | Widget")
	TObjectPtr<class UWidgetComponent> WaitingPlayerWidgetComp;


	TObjectPtr<class UWidgetComponent> ExitTimerWidgetComp;

	// 2D 위젯
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ConsoleObject | Widget")
	TSubclassOf<class UUserWidget> ExitTimerWidgetClass;

	UPROPERTY()
	TObjectPtr<class UUserWidget> ExitTimerWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="ConsoleObject | Widget")
	TSubclassOf<class UUserWidget> ElseExitTimerWidgetClass;

	UPROPERTY()
	TObjectPtr<class UUserWidget> ElseExitTimerWidget;
}; 
