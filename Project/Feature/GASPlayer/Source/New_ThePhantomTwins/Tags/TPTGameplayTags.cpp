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
#define TPT_TAG_GEN(Type, Name, Desc) \
    GameplayTags.TPTGamePlayTag_##Type##_##Name = Manager.AddNativeGameplayTag(TEXT("TPTGamePlayTag." #Type "." #Name), TEXT(Desc));    \
    if (!GameplayTags.TPTGamePlayTag_##Type##_##Name.IsValid())                                                                         \
    {                                                                                                                                   \
        GameplayTags.TPTGamePlayTag_##Type##_##Name = FGameplayTag::RequestGameplayTag(TEXT("TPTGamePlayTag." #Type "." #Name));        \
    }                                                                                                                                   
#include "TPTGameplayTagList.inl"
#undef TPT_TAG_GEN

#define TPT_TAG_GEN(Type, Name, Desc) \
    GameplayTags.TagMap.Add(GameplayTags.TPTGamePlayTag_##Type##_##Name, EFTPTGameplayTags::TPTGamePlayTag_##Type##_##Name);
#include "TPTGameplayTagList.inl"
#undef TPT_TAG_GEN

}