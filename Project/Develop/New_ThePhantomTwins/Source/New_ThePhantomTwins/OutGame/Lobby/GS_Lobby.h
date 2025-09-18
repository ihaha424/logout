// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Data/CharacterType.h"
#include "GS_Lobby.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSetIdentifyCharacterData, FIdentifyCharacterData, IdentifyCharacterData);

UCLASS()
class NEW_THEPHANTOMTWINS_API AGS_Lobby : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	void SetIdentifyCharacterData(ECharacterType ChractorType, bool bIsHost);
	UPROPERTY(BlueprintAssignable)
	FOnSetIdentifyCharacterData OnSetIdentifyCharacterData;

protected:
	UPROPERTY(ReplicatedUsing = OnRep_IdentifyCharacterData)
	FIdentifyCharacterData IdentifyCharacterData;
	UFUNCTION()
	void OnRep_IdentifyCharacterData();


	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
