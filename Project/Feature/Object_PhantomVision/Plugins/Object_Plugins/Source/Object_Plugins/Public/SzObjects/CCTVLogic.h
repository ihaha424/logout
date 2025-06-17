#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Net/UnrealNetwork.h"
#include "CCTVLogic.generated.h"

UCLASS()
class OBJECT_PLUGINS_API ACCTVLogic : public APawn
{
	GENERATED_BODY()
	
public:	
	ACCTVLogic();

protected:
	virtual void BeginPlay() override;

public:	
	void EnterFirstHackedCCTV(APawn* Interactor);

	// Client RPC - 지정된 소유자(Interactor의 PC)에게만 실행
	UFUNCTION(Client, Reliable)
	void ShowNoHackedCCTVUI_Client();
	void ShowNoHackedCCTVUI_Client_Implementation();

	UFUNCTION(Client, Reliable)
	void HideNoHackedCCTVUI_Client();
	void HideNoHackedCCTVUI_Client_Implementation();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="UI")
	TSubclassOf<UUserWidget> NoHackedCCTVWidget;
	
	TObjectPtr<class UUserWidget> CCTVWidgetInstance = nullptr;

private:
	TObjectPtr<class APhantomTwinsGameState> CurrentGameState;
	TObjectPtr<class UCCTVManager> CurrentCCTVManager;

	bool bIsWidgetShown = false;
};
