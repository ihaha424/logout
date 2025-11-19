// Fill out your copyright notice in the Description page of Project Settings.


#include "Utils/SimpleFileBPLibrary.h"
#include "SimpleFileBPLibrary.h"
#include "Misc/Paths.h"
#include "Misc/FileHelper.h"

bool USimpleFileBPLibrary::LoadCodeText(FString& OutText)
{
    // 절대경로: Saved/Data/code.txt
    const FString FilePath = FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("Data/code.txt"));
    OutText.Empty();

    if (FPaths::FileExists(FilePath))
    {
        return FFileHelper::LoadFileToString(OutText, *FilePath);
    }
    return false;
}
