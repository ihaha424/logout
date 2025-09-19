// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#define LOG_CALLINFO ANSI_TO_TCHAR(__FUNCTION__)
#define TPT_LOG(LogCat, Verbosity, Format, ...) UE_LOG(LogCat, Verbosity, TEXT("%s %s"), LOG_CALLINFO, *FString::Printf(Format, ##__VA_ARGS__))

DECLARE_LOG_CATEGORY_EXTERN(GALog, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(GELog, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(GCLog, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(TaskLog, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(TargetLog, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(PlayerLog, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(AttLog, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(ObjectLog, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(ItemLog, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(HUDLog, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(AILog, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(GameRuleLog, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(OutGameLog, Log, All);
DECLARE_LOG_CATEGORY_EXTERN(CMLog, Log, All);


/*		NULLCHECK_LOG		*/

/*		NULLCHECK_LOG Please close the '}'		*/
#define NULLCHECK_LOG(Target, CatecoryName, Verbosity) \
	if(nullptr == Target) { TPT_LOG(CatecoryName, Verbosity, TEXT(#Target " is nullptr."));

/*		NULLCHECK_LOG Please close the '}'		*/
#define NULLCHECK_CODE_LOG(Target, CatecoryName, Verbosity, CODE) \
	NULLCHECK_LOG(Target, CatecoryName, Verbosity) \
		CODE


#define NULLCHECK_RETURN_LOG(Target, CatecoryName, Verbosity, ReturnValue) \
	NULLCHECK_LOG(Target, CatecoryName, Verbosity) \
		return ReturnValue; \
	}

#define NULLCHECK_CODE_RETURN_LOG(Target, CatecoryName, Verbosity, CODE, ReturnValue) \
	NULLCHECK_CODE_LOG(Target, CatecoryName, Verbosity, CODE) \
		return ReturnValue; \
	}

/*		CONDITIONCHECK_LOG		*/

#define CONDITIONCHECK_RETURN_LOG(Condition, CatecoryName, Verbosity, ReturnValue) \
	if(Condition) { TPT_LOG(CatecoryName, Verbosity, TEXT(#Condition " is invalid.")); \
		return ReturnValue; \
	}

#define CONDITIONCHECK_CODE_RETURN_LOG(Condition, CatecoryName, Verbosity, CODE, ReturnValue) \
	if(Condition) { TPT_LOG(CatecoryName, Verbosity, TEXT(#Condition " is invalid.")); \
		CODE \
		return ReturnValue; \
	}

