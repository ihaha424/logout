// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "UIManager/UIManager.h"
#include "PC_Player.generated.h"

class UInputMappingContext;
/**
 * 
 */
UCLASS()
class NEW_THEPHANTOMTWINS_API APC_Player : public APlayerController
{
	GENERATED_BODY()

public:
	static APC_Player* GetLocalPlayerController(AActor* Actor);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> IMC;

	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;

	/**
	 * @brief	
			: Functions that give ownership to a particular object in the client
	 * @param thisPC 
			: Client PC
	 * @param Actor 
			: Ownership Target
	 */
	UFUNCTION(Server, Reliable)
	void C2S_SetOwnerActor(APlayerController* thisPC, AActor* Actor);
	void C2S_SetOwnerActor_Implementation(APlayerController* thisPC, AActor* Actor);



	//////////////////////////////////////////////////////////////////////////////////////////
	//																						//
	//									UI Manager											//
	//																						//
	//////////////////////////////////////////////////////////////////////////////////////////
	UFUNCTION(BlueprintCallable, Category = "UIManager")
	void SetWidget(const FString& UIKey, bool bActive, EMessageTargetType TargetType);
	UFUNCTION(BlueprintCallable, Category = "UIManager")
	UUserWidget* GetWidget(const FString& UIKey) const;
	UFUNCTION(BlueprintCallable, Category = "UIManager")
	void RegisterWidget(const FString& Key, UUserWidget* Widget, int32 Order = 0);
	UFUNCTION(BlueprintCallable, Category = "UIManager")
	void UnregisterWidget(const FString& Key, UUserWidget* Widget);


protected:
	UPROPERTY(BlueprintReadWrite, Category = "UIManager")
	TObjectPtr<UUIManager> UIManager;

private:

	//////////////////////////////////////////////////////////////////////////////////////////
	//																						//
	//									UI Manager											//
	//																						//
	//////////////////////////////////////////////////////////////////////////////////////////
	void NetSetWidget(const FString& UIKey, bool bActive, EMessageTargetType TargetType);
	inline void HandleServerSetWidget(const FString& UIKey, bool bActive, EMessageTargetType TargetType);
	inline void HandleClientSetWidget(const FString& UIKey, bool bActive, EMessageTargetType TargetType);
	// Clinet ¡æ Sever Request.
	UFUNCTION(Server, Reliable)
	void C2S_ShowUI(const FString& UIKey, bool bActive, EMessageTargetType TargetType);
	void C2S_ShowUI_Implementation(const FString& UIKey, bool bActive, EMessageTargetType TargetType);
	// Sever ¡æ Clinet Command
	UFUNCTION(Client, Reliable)
	void S2C_ShowUI(const FString& UIKey, bool bActive);
	void S2C_ShowUI_Implementation(const FString& UIKey, bool bActive);
};
