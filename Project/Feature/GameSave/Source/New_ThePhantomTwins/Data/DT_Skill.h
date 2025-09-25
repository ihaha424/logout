// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "DT_Skill.generated.h"

UENUM(BlueprintType)
enum class ESkillType : uint8
{
	NoneSkill,               // None
    GiveNoiseBomb,      // 소음발생기 +1
    GiveEMP,            // EMP +1
    GiveHealPack,       // 힐팩 +2
    GiveMentalPack,     // 멘탈팩 +2
    GiveKey,            // 열쇠 +1
    MaxHealthUp,        // 최대 체력 30 추가
    MaxMentalUp,        // 최대 정신력 30 추가
    MaxStaminaUp,       // 최대 스태미나 30 추가
};

USTRUCT(BlueprintType)
struct FSkillDataTableRow : public FTableRowBase
{
    GENERATED_BODY()

public:
    FSkillDataTableRow()
	    : SkillType(ESkillType::NoneSkill)
		, SkillName()
		, SkillIcon(nullptr)
		, SkillDescription()
    {}

    // 스킬 타입 (Enum)
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    ESkillType SkillType;

    // 스킬이름
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FText SkillName;

    // 스킬 아이콘
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UTexture2D* SkillIcon;

    // 스킬설명
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FText SkillDescription;
};

UCLASS()
class NEW_THEPHANTOMTWINS_API UDT_Skill : public UDataTable
{
	GENERATED_BODY()
	
};
