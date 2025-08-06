#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Engine/Texture2D.h"
#include "PlayerStatusWidget.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class NEW_THEPHANTOMTWINS_API UPlayerStatusWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
    virtual void NativeConstruct() override;

public:
    void SetHP(int32 HP);
    void SetMental(int32 Mental);
    void SetCharPortrait(UTexture2D* PortraitTexture);

protected:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<class UProgressBar> HPBar;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<class UProgressBar> MentalBar;

    UPROPERTY(meta = (BindWidget))
    TObjectPtr<class UImage> CharPortrait;

public:
    UPROPERTY()
    float MaxHP = 100.0f;

    UPROPERTY()
    float MaxMental = 100.0f;
};
