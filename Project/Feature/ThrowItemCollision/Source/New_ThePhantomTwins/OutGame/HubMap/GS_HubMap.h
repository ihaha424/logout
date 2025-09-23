// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Data/CharacterType.h"
#include "Data/MapType.h"
#include "GS_HubMap.generated.h"


UENUM(BlueprintType)
enum class EHubMapState : uint8
{
	None		UMETA(DisplayName = "None"),
	Title		UMETA(DisplayName = "Title"),
	Lobby		UMETA(DisplayName = "Select Character"),
	Hub_Main    UMETA(DisplayName = "HubMap Main"),
	Hub_Map     UMETA(DisplayName = "HubMap Map Select"),
	Hub_Skill   UMETA(DisplayName = "HubMap Skill Select"),
	Hub_Table   UMETA(DisplayName = "HubMap Table"),
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSetIdentifyCharacterData, FIdentifyCharacterData, IdentifyCharacterData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FHubMapChanged, EHubMapState, CurState, EHubMapState, NextState);

UCLASS()
class NEW_THEPHANTOMTWINS_API AGS_HubMap : public AGameStateBase
{
	GENERATED_BODY()
public:
	// ~Begin HubMap
	UFUNCTION(BlueprintCallable, Category = "HubMap")
	void SetCurState(EHubMapState State) { OnChangeHubMapState.Broadcast(CurState, State); CurState = State; }
	UFUNCTION(BlueprintCallable, Category = "HubMap")
	EHubMapState GetCurState() const { return CurState; }
	UFUNCTION(BlueprintCallable, Category = "HubMap")
	void SetNextLevel(FName State) { NextLevel = State; }
	UFUNCTION(BlueprintCallable, Category = "HubMap")
	FName GetNextLevel() const { return NextLevel; }
	UFUNCTION(BlueprintCallable, Category = "HubMap")
	void SetMapType(EMapType State) { MapData = State; OnRep_MapData(); }
	UFUNCTION(BlueprintCallable, Category = "HubMap")
	EMapType GetMapType() const { return MapData; }
	// ~End HubMap

	// ~Begin Lobby(Character Seletect)
	void SetIdentifyCharacterData(ECharacterType ChractorType, bool bIsHost);
	UPROPERTY(BlueprintAssignable, Category = "HubMap | Character Seletect")
	FSetIdentifyCharacterData OnSetIdentifyCharacterData;
	// ~End Lobby(Character Seletect)

private:
	// ~Begin HubMap
	EHubMapState CurState;
	UPROPERTY(BlueprintAssignable, Category = "HubMap")
	FHubMapChanged OnChangeHubMapState;
	UPROPERTY(Replicated)
	FName NextLevel;
	UPROPERTY(ReplicatedUsing = OnRep_MapData)
	EMapType MapData;
	UFUNCTION()

	void OnRep_MapData();
	// ~End HubMap


	// ~Begin Lobby(Character Seletect)
	UPROPERTY(ReplicatedUsing = OnRep_IdentifyCharacterData)
	FIdentifyCharacterData IdentifyCharacterData;
	UFUNCTION()
	void OnRep_IdentifyCharacterData();
	// ~End Lobby(Character Seletect)

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
