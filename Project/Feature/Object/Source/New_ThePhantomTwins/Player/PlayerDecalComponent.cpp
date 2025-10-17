// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/PlayerDecalComponent.h"
#include "Decal/StickerActor.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Engine/Player.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "Decal/StickerLibrary.h"
#include "GS_PhantomTwins.h"
#include "Decal/StickerManager.h"
#include "GameFramework/PlayerState.h"

#include "Log/TPTLog.h"


UPlayerDecalComponent::UPlayerDecalComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void UPlayerDecalComponent::BeginPlay()
{
	Super::BeginPlay();
}


void UPlayerDecalComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

bool UPlayerDecalComponent::TryPlaceSticker(int32 EmojiId, float Size, float Lifetime, bool bAttachToMovers)
{
    APawn* Pawn = Cast<APawn>(GetOwner());
    NULLCHECK_RETURN_LOG(Pawn, PlayerLog, Warning, false);

    APlayerController* PC = Cast<APlayerController>(Pawn->GetController());
    NULLCHECK_RETURN_LOG(PC, PlayerLog, Warning, false);

    FVector CamLoc; FRotator CamRot;
    PC->GetPlayerViewPoint(CamLoc, CamRot);

    const FVector Start = CamLoc;
    const FVector End = Start + CamRot.Vector() * 2000.f;

    FHitResult Hit;
    FCollisionQueryParams P(SCENE_QUERY_STAT(StickerTrace), true);
    P.AddIgnoredActor(Pawn);

    if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, P))
    {
        FStickerParams Params{ EmojiId, Size, FLinearColor::White, Lifetime };
        PlaceSticker(Hit, Params, bAttachToMovers);
        return true;
    }
    return false;
}

void UPlayerDecalComponent::PlaceSticker(const FHitResult& Hit, const FStickerParams& Params, bool bAttach)
{
    if (!Hit.bBlockingHit || !StickerActorClass) return;

    const AActor* InstigatorActor = GetOwner();
    const APlayerController* PC = InstigatorActor->GetInstigatorController<APlayerController>();
    if (!InstigatorActor) return;

    if (FVector::Dist(Hit.TraceStart, Hit.ImpactPoint) > MaxDist) return;

    AStickerActor* SA = GetWorld()->SpawnActorDeferred<AStickerActor>(
        StickerActorClass, FTransform::Identity, nullptr, nullptr,
        ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

    SA->Init(Params);
    SA->FinishSpawning(FTransform::Identity);
    SA->PlaceOnHit(Hit, PC, bAttach);
    AGS_PhantomTwins* GS = GetWorld()->GetGameState<AGS_PhantomTwins>();
    if (GS)
    {
        const int OwnerPlayerId = PC->PlayerState ? PC->PlayerState->GetPlayerId() : -1;
        GS->GetStickerManager()->RegisterSticker(OwnerPlayerId, SA);
    }
}

