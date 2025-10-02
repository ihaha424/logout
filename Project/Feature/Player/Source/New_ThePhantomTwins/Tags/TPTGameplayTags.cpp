// Fill out your copyright notice in the Description page of Project Settings.


#include "TPTGameplayTags.h"
#include "GameplayTagsManager.h"
#include "TPTGameplayTagExporter.h"

FTPTGameplayTags FTPTGameplayTags::GameplayTags;
bool FTPTGameplayTags::bIsInitial = false;

const FTPTGameplayTags& FTPTGameplayTags::Get()
{
    if (!bIsInitial)
    {
        FTPTGameplayTags::InitializeNativeTags();
        bIsInitial = true;
    }

    return GameplayTags;
}

void FTPTGameplayTags::InitializeNativeTags()
{
    TPTGameplayTagExporter::ExportTagsToIni();

    // GameplayTags를 총괄하는 싱글톤 매니저
    UGameplayTagsManager& Manager = UGameplayTagsManager::Get();


    /**
     *  태그는 Project Settings > GameplayTags에 자동으로 추가
            Ex> InputTag.Interact태그 추가, Input: Interact (F) 설명용 메타 정보
            -> GameplayTags.TPTGamePlayTag_InputTag_Interact = Manager.AddNativeGameplayTag(TEXT("TPTGamePlayTag.InputTag.Interact"), TEXT("Input: Interact (F)"));
    **/
//
// ADD_NATIVE_GAMEPLAYTAG_GEN
//
#define TAG true
#define TPT_TAG_GEN(Type, Properties, Name, Description) \
    GameplayTags.TPTGameplay_##Type##_##Properties##_##Name = Manager.AddNativeGameplayTag(TEXT("TPTGamePlayTag." #Type "." #Properties "." #Name), TEXT(Description));   \
    if (!GameplayTags.TPTGameplay_##Type##_##Properties##_##Name.IsValid())                                                             \
    {                                                                                                                       \
        GameplayTags.TPTGameplay_##Type##_##Properties##_##Name = FGameplayTag::RequestGameplayTag(TEXT("TPTGamePlayTag." #Type "." #Properties "." #Name));       \
    }                                                                                                                                   
#include "./Inline/TPTGameplayTagList.inl"
#undef TPT_TAG_GEN
#undef TAG

#define CUETAG true
#define TPT_TAG_GEN(Type, Name, Description) \
    GameplayTags.GameplayCue_##Type##_##Name = Manager.AddNativeGameplayTag(TEXT("GamePlayCue." #Type "." #Name), TEXT(Description));   \
    if (!GameplayTags.GameplayCue_##Type##_##Name.IsValid())                                                             \
    {                                                                                                                       \
        GameplayTags.GameplayCue_##Type##_##Name = FGameplayTag::RequestGameplayTag(TEXT("GamePlayCue." #Type "." #Name));       \
    }                                                                                                                                   
#include "./Inline/TPTGameplayTagList.inl"
#undef TPT_TAG_GEN
#undef CUETAG

//
// BIND_TAG_TO_ENUM
//
#define TAG true
#define TPT_TAG_GEN(Type, Properties, Name, Description) \
    GameplayTags.TagMap.Add(GameplayTags.TPTGameplay_##Type##_##Properties##_##Name, EFTPTGameplayTags::TPTGameplay_##Type##_##Properties##_##Name);
#include "./Inline/TPTGameplayTagList.inl"
#undef TPT_TAG_GEN
#undef TAG

#define CUETAG true
#define TPT_TAG_GEN(Type, Name, Description) \
    GameplayTags.TagMap.Add(GameplayTags.GameplayCue_##Type##_##Name, EFTPTGameplayTags::GameplayCue_##Type##_##Name);
#include "./Inline/TPTGameplayTagList.inl"
#undef TPT_TAG_GEN
#undef CUETAG

	for (const auto& Pair : GameplayTags.TagMap)
	{
		GameplayTags.EnumMap.Add(Pair.Value, Pair.Key);
	}
}