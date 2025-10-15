#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "StickerActor.generated.h"

class UDecalComponent;
class UMaterialInterface;
class UMaterialInstanceDynamic;
class UStickerLibrary;

USTRUCT(BlueprintType)
struct FStickerParams
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 EmojiId = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Size = 32.f; // half extents

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FLinearColor Tint = FLinearColor::White;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Lifetime = 0.f; // 0 = Infinity
};

UCLASS()
class AStickerActor : public AActor
{
    GENERATED_BODY()
public:
    AStickerActor();

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    /**
     * @brief : Initialize is only Sever.
     */
    void Init(const FStickerParams& InParams);

protected:
    UPROPERTY(EditDefaultsOnly, Category = "Sticker")
    TObjectPtr<UStickerLibrary> StickerLib;

    UPROPERTY(VisibleAnywhere)
    TObjectPtr<UDecalComponent> DecalComp;

    UPROPERTY(ReplicatedUsing = OnRep_Params)
    FStickerParams Params;

    UPROPERTY()
    TObjectPtr<UMaterialInstanceDynamic> DMI;

    UFUNCTION()
    void OnRep_Params();

    void ApplyParams();

    // Use to attach to a moving object (optional)
    UPROPERTY(Replicated)
    TWeakObjectPtr<UPrimitiveComponent> AttachedComp;

public:
    // Processing of world position/rotation/attachment with hit information
    void PlaceOnHit(const FHitResult& Hit, const APlayerController* PC, bool bAttachToHitComponent);

private:
    const float DecalZVolume = 1.f;
};
