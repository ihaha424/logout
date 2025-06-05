// Fill out your copyright notice in the Description page of Project Settings.


#include "PhantomTwinsInstance.h"
#include "StoryFlow/StoryFlowManager.h"

UStoryFlowManager* UPhantomTwinsInstance::GetStoryFlowManager()
{
	return GetSubsystem<UStoryFlowManager>();
}
