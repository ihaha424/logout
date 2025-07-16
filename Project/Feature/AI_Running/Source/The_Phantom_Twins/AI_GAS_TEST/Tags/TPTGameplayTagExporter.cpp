// Fill out your copyright notice in the Description page of Project Settings.


#include "TPTGameplayTagExporter.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "HAL/FileManager.h"

#define TPT_TAG_GEN(Type, Name, Desc) TEXT("GameplayTagList=(Tag=\"TPTGamePlayTag." #Type "." #Name "\", DevComment=\"" Desc "\")\n")
static const TCHAR* GeneratedTagLines =
#include "TPTGameplayTagList.inl"
;

#undef TPT_TAG_GEN


void TPTGameplayTagExporter::ExportTagsToIni()
{
    FString ExportPath = FPaths::ProjectConfigDir() + TEXT("Tags/GeneratedGameplayTags.ini");

    if (FFileHelper::SaveStringToFile(TEXT("<code class=\"language - cpp\">\n[/Script/GameplayTags.GameplayTagsList]\n") + FString(GeneratedTagLines) + TEXT("</code>"), *ExportPath))
    {
        UE_LOG(LogTemp, Warning, TEXT("Gameplay Tags exported to %s"), *ExportPath);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to write Gameplay Tags to %s"), *ExportPath);
    }
}
