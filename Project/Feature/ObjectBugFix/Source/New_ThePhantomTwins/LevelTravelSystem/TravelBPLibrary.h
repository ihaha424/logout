// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "TravelBPLibrary.generated.h"

/**
 * 
 */
UCLASS()
class NEW_THEPHANTOMTWINS_API UTravelBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
    /** 바로 사용 가능한 최상위 BP 노드 */
    UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
    static void TravelWithWidget(const UObject* WorldContextObject,
        const FString& TargetMap,
        TSubclassOf<UUserWidget> WidgetClass,
        bool bServerTravel = true);

    // 기존 ServerTravel / ClientTravel 그대로 포함
    static bool ServerTravel(const UObject* WorldContextObject,
        const FString& MapPath,
        bool bUseSeamless = true,
        bool bKeepListen = true,
        const FString& ExtraOptions = TEXT(""),
        bool bAbsolute = false,
        bool bSkipGameNotify = false);

    static void ClientTravel(APlayerController* PC,
        const FString& URL,
        TEnumAsByte<ETravelType> TravelType = ETravelType::TRAVEL_Absolute,
        bool bSeamless = false);
};
