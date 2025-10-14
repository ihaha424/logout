// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "StickerLibrary.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class NEW_THEPHANTOMTWINS_API UStickerLibrary : public UDataAsset
{
	GENERATED_BODY()
public:
    // 머티리얼 내부에서 EmojiId로 TextureArray/Curve/Flipbook 등 선택
    UPROPERTY(EditAnywhere)
    TObjectPtr<UMaterialInterface> BaseDecalMaterial = nullptr;

    // 선택적: 미리보기/메뉴용
    UPROPERTY(EditAnywhere)
    TArray<TSoftObjectPtr<UTexture2D>> EmojiPreview;
};
