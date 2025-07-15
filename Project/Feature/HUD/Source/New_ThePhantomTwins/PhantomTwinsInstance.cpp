// Fill out your copyright notice in the Description page of Project Settings.


#include "PhantomTwinsInstance.h"
#include "Tags/TPTGameplayTags.h"

void UPhantomTwinsInstance::Init()
{
	Super::Init();
	FTPTGameplayTags::Get();
}
