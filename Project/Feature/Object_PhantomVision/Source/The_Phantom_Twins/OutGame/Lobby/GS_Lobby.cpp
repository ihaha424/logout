// Fill out your copyright notice in the Description page of Project Settings.


#include "GS_Lobby.h"
#include "../../StoryFlow/StoryFlowManager.h"

void AGS_Lobby::S2A_SelectChractorEffect_Implementation(ECharacterType ChractorType, bool bIsHost, const FName& DataName)
{
    UStoryFlowManager* Manager = GetGameInstance()->GetSubsystem<UStoryFlowManager>();
    
    UIdentifyChracterData* ChractorData = Manager->GetDataAs<UIdentifyChracterData>(DataName);
    if (!ChractorData)
    {
        UE_LOG(LogStoryFlow, Error, TEXT("GetDataAs<UIdentifyChracterData> Faild. Data name is %s"), *DataName.ToString());
        return;
    }

    if (bIsHost)
        ChractorData->Host = ChractorType;
    else
        ChractorData->Client = ChractorType;

    Manager->SetData(DataName, ChractorData);

}

