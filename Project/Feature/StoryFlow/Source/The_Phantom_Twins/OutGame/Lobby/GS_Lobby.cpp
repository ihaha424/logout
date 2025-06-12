// Fill out your copyright notice in the Description page of Project Settings.


#include "GS_Lobby.h"
#include "../../Chapter/IdentifyChracterData.h"
#include "../../StoryFlow/StoryFlowManager.h"

void AGS_Lobby::S2A_SelectChractorEffect_Implementation(UIdentifyChracterData* ChractorData, const FName& DataName)
{
    UStoryFlowManager* Manager = GetGameInstance()->GetSubsystem<UStoryFlowManager>();

    Manager->SetData(DataName, ChractorData);
}
