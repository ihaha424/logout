// Fill out your copyright notice in the Description page of Project Settings.


#include "PhantomTwinsInstance.h"
#include "StoryFlow/StoryFlowManager.h"
#include "AI_GAS_TEST/Tags/TPTGameplayTags.h"

void UPhantomTwinsInstance::Init()
{
	FTPTGameplayTags::Get();
}

UStoryFlowManager* UPhantomTwinsInstance::GetStoryFlowManager()
{
	return GetSubsystem<UStoryFlowManager>(); // GameInstanceSubSystem
}
