// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#define LOG_CALLINFO ANSI_TO_TCHAR(__FUNCTION__)
#define TPT_LOG(LogCat, Verbosity, Format, ...) UE_LOG(LogCat, Verbosity, TEXT("%s %s"), LOG_CALLINFO, *FString::Printf(Format, ##__VA_ARGS__))

DECLARE_LOG_CATEGORY_EXTERN(GALog, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(GELog, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(PlayerLog, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(ObjectLog, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(HUDLog, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(AILog, Log, All);