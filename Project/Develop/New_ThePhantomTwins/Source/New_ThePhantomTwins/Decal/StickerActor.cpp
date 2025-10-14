// Fill out your copyright notice in the Description page of Project Settings.


#include "Decal/StickerActor.h"
#include "Components/DecalComponent.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/KismetMathLibrary.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Decal/StickerLibrary.h"

#include "Kismet/KismetSystemLibrary.h"

AStickerActor::AStickerActor()
{
    bReplicates = true;
    SetReplicateMovement(true);

    DecalComp = CreateDefaultSubobject<UDecalComponent>(TEXT("DecalComp"));
    RootComponent = DecalComp;

    DecalComp->SetFadeScreenSize(0.001f);        // 멀리서 자동 페이드
    DecalComp->SetRelativeLocation(FVector::ZeroVector);
    DecalComp->DecalSize = FVector(32.f);        // 기본값
}

void AStickerActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AStickerActor, Params);
    DOREPLIFETIME(AStickerActor, AttachedComp);
}

void AStickerActor::Init(const FStickerParams& InParams)
{
    check(HasAuthority());

    if (!StickerLib || !StickerLib->BaseDecalMaterial) return;
    Params = InParams;

    UMaterialInterface* BaseDecalMat = StickerLib->BaseDecalMaterial;
    if (BaseDecalMat)
    {
        DecalComp->SetDecalMaterial(BaseDecalMat);
        // X=Depth, Y=Width, Z=Height (원하면 비정방형 지원)
        DecalComp->DecalSize = FVector(DecalZVolume, Params.Size, Params.Size);
        DMI = DecalComp->CreateDynamicMaterialInstance();
    }
    ApplyParams();

    if (Params.Lifetime > 0.f)
    {
        SetLifeSpan(Params.Lifetime);
    }
}

void AStickerActor::OnRep_Params()
{
    UMaterialInterface* BaseDecalMat = StickerLib->BaseDecalMaterial;
    if (BaseDecalMat)
    {
        DecalComp->SetDecalMaterial(BaseDecalMat);
        DecalComp->DecalSize = FVector(DecalZVolume, Params.Size, Params.Size);
        DMI = DecalComp->CreateDynamicMaterialInstance();
        ApplyParams();
    }
}

void AStickerActor::ApplyParams()
{
    if (DMI)
    {
        //// 머티리얼 파라미터 명은 네가 쓰는 머티리얼에 맞춰서:
        //DMI->SetScalarParameterValue(TEXT("ID"), (float)Params.EmojiId);
        //DMI->SetVectorParameterValue(TEXT("Color"), Params.Tint);
        if (StickerLib->EmojiPreview.IsValidIndex(Params.EmojiId))
        {
            UTexture2D* Tex = StickerLib->EmojiPreview[Params.EmojiId].LoadSynchronous();
            DMI->SetTextureParameterValue(TEXT("DecalImage"), Tex);
        }
    }
}

void AStickerActor::PlaceOnHit(const FHitResult& Hit, bool bAttachToHitComponent)
{
    const FVector N = Hit.ImpactNormal.GetSafeNormal();

    // 회전: 데칼의 +X가 표면 안쪽(-N)으로 향하도록
    const FRotator Rot = UKismetMathLibrary::MakeRotFromX(-N);
    SetActorRotation(Rot);

    // 위치: ImpactPoint에서 '깊이의 절반 - epsilon'만큼 표면 바깥쪽(+N)으로
    const float Depth = (DecalComp ? DecalComp->DecalSize.X : 32.f);
    const float Eps = 0.5f; // Z-fight 방지 미세 오프셋
    const FVector Loc = Hit.ImpactPoint + N * (Depth * 0.5f - Eps);
    SetActorLocation(Loc);

    if (bAttachToHitComponent && Hit.GetComponent())
    {
        AttachToComponent(Hit.GetComponent(), FAttachmentTransformRules::KeepWorldTransform);
        AttachedComp = Hit.GetComponent();
    }
}