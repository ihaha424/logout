// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Decal/StickerActor.h"
#include "PlayerDecalComponent.generated.h"


class AStickerActor;
class UStickerLibrary;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class NEW_THEPHANTOMTWINS_API UPlayerDecalComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UPlayerDecalComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/**
	 * @brief : Only Excute Play Sever.
	 */
	UFUNCTION(BlueprintCallable)
	bool TryPlaceSticker(int32 EmojiId, float Size, float Lifetime, bool bAttachToMovers);
public:
    UPROPERTY(EditDefaultsOnly, Category = "Sticker")
    TSubclassOf<AStickerActor> StickerActorClass;

protected:
    void PlaceSticker(const FHitResult& Hit, const FStickerParams& Params, bool bAttach);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sticker")
	float MaxDist = 2200.f;
};


/*
	4) 세이브/로드
	서버에서 TArray<FTransform + FStickerParams + AttachedComp(SoftRef)> 보관
	맵 로드시 서버가 순회 복원 → PlaceOnHit 대신 Transform/Attach로 재구성
	AttachedComp는 소프트 참조(이름/경로)로 저장하거나, 못 찾으면 월드 고정으로 대체
*/