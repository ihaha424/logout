// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

/**
 *  Tag 추가 방법
            : TPTGameplayTagList.inl를 참고하여 추가하면 됨.
 */

UENUM()
enum class EFTPTGameplayTags : uint8
{
//
// ENUM_TAG_GEN
//
#define TAG true
#define TPT_TAG_GEN(Type, Properties, Name, Description) TPTGameplay_##Type##_##Properties##_##Name UMETA(DisplayName = Description),
#include "./Inline/TPTGameplayTagList.inl"
#undef TPT_TAG_GEN
#undef TAG

#define CUETAG true
#define TPT_TAG_GEN(Type, Name, Description) GameplayCue_##Type##_##Name UMETA(DisplayName = Description),
#include "./Inline/TPTGameplayTagList.inl"
#undef TPT_TAG_GEN
#undef CUETAG
};

struct FTPTGameplayTags
{
public:
    static const FTPTGameplayTags& Get();
    static void InitializeNativeTags();

//
// GAMEPLAY_TAG_GEN
//
#define TAG true
#define TPT_TAG_GEN(Type, Properties, Name, Description) FGameplayTag TPTGameplay_##Type##_##Properties##_##Name;
#include "./Inline/TPTGameplayTagList.inl"
#undef TPT_TAG_GEN
#undef TAG

#define CUETAG true
#define TPT_TAG_GEN(Type, Name, Description) FGameplayTag GameplayCue_##Type##_##Name;
#include "./Inline/TPTGameplayTagList.inl"
#undef TPT_TAG_GEN
#undef CUETAG

    TMap<FGameplayTag, EFTPTGameplayTags> TagMap;
private:
    static FTPTGameplayTags GameplayTags;
    static bool bIsInitial;
};

