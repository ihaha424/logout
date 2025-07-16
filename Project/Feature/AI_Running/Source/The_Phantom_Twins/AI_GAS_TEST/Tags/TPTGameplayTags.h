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
#define TPT_TAG_GEN(Type, Name, Desc) TPTGamePlayTag_##Type##_##Name UMETA(DisplayName = Desc),
#include "TPTGameplayTagList.inl"
#undef TPT_TAG_GEN
};

struct FTPTGameplayTags
{
public:
    static const FTPTGameplayTags& Get();
    static void InitializeNativeTags();

#define TPT_TAG_GEN(Type, Name, Desc) FGameplayTag TPTGamePlayTag_##Type##_##Name;
#include "TPTGameplayTagList.inl"
#undef TPT_TAG_GEN
    //FGameplayTag TPTGamePlayTag_Type_Name;

    TMap<FGameplayTag, EFTPTGameplayTags> TagMap;
private:
    static FTPTGameplayTags GameplayTags;
    static bool bIsInitial;
};

