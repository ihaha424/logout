// Fill out your copyright notice in the Description page of Project Settings.


#include "GS_PhantomTwins.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"

void AGS_PhantomTwins::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AGS_PhantomTwins, GameTime);
    DOREPLIFETIME(AGS_PhantomTwins, CoreCount);
}
