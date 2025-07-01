// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "../../CharacterType.h"
#include "GS_Lobby.generated.h"

USTRUCT(BlueprintType)
struct NEW_THEPHANTOMTWINS_API FIdentifyChracterData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	ECharacterType Host = ECharacterType::None;

	UPROPERTY(BlueprintReadOnly)
	ECharacterType Client = ECharacterType::None;
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSetIdentifyChracterData, FIdentifyChracterData, IdentifyChracterData);

UCLASS()
class NEW_THEPHANTOMTWINS_API AGS_Lobby : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	void SetIdentifyChracterData(ECharacterType ChractorType, bool bIsHost);
	UPROPERTY(BlueprintAssignable)
	FOnSetIdentifyChracterData OnSetIdentifyChracterData;

protected:
	UPROPERTY(ReplicatedUsing = OnRep_IdentifyChracterData)
	FIdentifyChracterData IdentifyChracterData;
	UFUNCTION()
	void OnRep_IdentifyChracterData();


	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
