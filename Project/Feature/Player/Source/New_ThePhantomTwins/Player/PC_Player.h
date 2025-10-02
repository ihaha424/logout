// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UIManagerPlayerController.h"
#include "PC_Player.generated.h"

class UInputMappingContext;
class APS_Player;

UCLASS()
class NEW_THEPHANTOMTWINS_API APC_Player : public AUIManagerPlayerController
{
	GENERATED_BODY()

public:
	APC_Player();

	static APC_Player* GetLocalPlayerController(AActor* Actor);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> IMC;

	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;

	/**
	 * @brief	
			: Functions that give ownership to a particular object in the client
	 * @param thisPC 
			: Client PC
	 * @param Actor 
			: Ownership Target
	 */
	//UFUNCTION(BlueprintCallable, Server, Reliable)
	//void C2S_TurnOffESC();
	//void C2S_TurnOffESC_Implementation();
	UFUNCTION(BlueprintCallable)
	void TurnOffESC();

	UFUNCTION(Server, Reliable)
	void C2S_SetOwnerActor(APlayerController* thisPC, AActor* Actor);
	void C2S_SetOwnerActor_Implementation(APlayerController* thisPC, AActor* Actor);

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void C2S_ClickedRestart(const bool bIsClicked);
	void C2S_ClickedRestart_Implementation(const bool bIsClicked);

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void C2S_ClickedGameStop(const FName LevelName);
	void C2S_ClickedGameStop_Implementation(const FName LevelName);

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void C2S_ClickedAgreeWithGameStop(const int32 SelectBtn);
	void C2S_ClickedAgreeWithGameStop_Implementation(const int32 SelectBtn);

};
