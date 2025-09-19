// Fill out your copyright notice in the Description page of Project Settings.


#include "TPTLog.h"
#include "Modules/ModuleManager.h"

DEFINE_LOG_CATEGORY(GALog);
DEFINE_LOG_CATEGORY(GELog);
DEFINE_LOG_CATEGORY(GCLog);
DEFINE_LOG_CATEGORY(TargetLog);
DEFINE_LOG_CATEGORY(TaskLog);
DEFINE_LOG_CATEGORY(PlayerLog);
DEFINE_LOG_CATEGORY(AttLog);
DEFINE_LOG_CATEGORY(ObjectLog);
DEFINE_LOG_CATEGORY(ItemLog);
DEFINE_LOG_CATEGORY(HUDLog);
DEFINE_LOG_CATEGORY(AILog);
DEFINE_LOG_CATEGORY(GameRuleLog);
DEFINE_LOG_CATEGORY(OutGameLog);
DEFINE_LOG_CATEGORY(CMLog);

IMPLEMENT_GAME_MODULE(FDefaultGameModuleImpl, New_ThePantomTwins);