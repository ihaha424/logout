// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "UIManagerPlayerController.h"
#include "OutGame/HubMap/GS_HubMap.h"
#include "Data/MapType.h"
#include "Data/DT_Skill.h"
#include "PC_HubMap.generated.h"

/**
 *
 */
UCLASS()
class NEW_THEPHANTOMTWINS_API APC_HubMap : public AUIManagerPlayerController
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "HubMap | Character Seletect")
	void C2S_SelectCharacter(const ECharacterType type);
	void C2S_SelectCharacter_Implementation(const ECharacterType type);

	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "HubMap | Skill Seletect")
	void C2S_SelectSkill(const ESkillType type);
	void C2S_SelectSkill_Implementation(const ESkillType type);

	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "HubMap | Is Ready")
	void C2S_CallReady(bool bIsReady);
	void C2S_CallReady_Implementation(bool bIsReady);

	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "HubMap | State Seletect")
	void C2S_SelectState(const EHubMapState type);
	void C2S_SelectState_Implementation(const EHubMapState type);

	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "HubMap | Map Seletect")
	void C2S_SelectMap(const FName& type, EMapType State);
	void C2S_SelectMap_Implementation(const FName& type, EMapType State);

	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "HubMap | LevelTravel")
	void C2S_LevelTravel(const FName& type, bool bAbsolute);
	void C2S_LevelTravel_Implementation(const FName& type, bool bAbsolute);
};
