// Fill out your copyright notice in the Description page of Project Settings.


#include "Decal/StickerActor.h"
#include "Components/DecalComponent.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/KismetMathLibrary.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Decal/StickerLibrary.h"

int AStickerActor::SortOrder = 0;

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

void AStickerActor::Init(const FStickerParams& InParams, const APlayerController* PC)
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
        DecalComp->SortOrder = SortOrder++;
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

void AStickerActor::PlaceOnHit(const FHitResult& Hit, const APlayerController* PC, bool bAttachToHitComponent)
{
    if (!PC)    
        return;
    const FVector N = Hit.ImpactNormal.GetSafeNormal();

    // 플레이어 카메라 방향 및 Up 벡터
    FVector CamLoc;
    FRotator CamRot;

    PC->GetPlayerViewPoint(CamLoc, CamRot);
    const FVector CamForward = CamRot.Vector();
    const FVector CamUp = FRotationMatrix(CamRot).GetScaledAxis(EAxis::Z);

    // 카메라 Up을 표면에 투영
    FVector TangentUp = (CamUp - FVector::DotProduct(CamUp, N) * N).GetSafeNormal();
    if (TangentUp.IsNearlyZero())
    {
        // 카메라가 표면에 수직이면 Forward 사용
        TangentUp = (CamForward - FVector::DotProduct(CamForward, N) * N).GetSafeNormal();
    }

    // 회전 생성: +X는 표면 안쪽(-N), +Z는 카메라 기준 위쪽
    FRotator Rot = UKismetMathLibrary::MakeRotFromXZ(-N, TangentUp);
    Rot.Roll += 90.f; // +90.f 또는 -90.f
    SetActorRotation(Rot);

    // 위치 보정
    const float Depth = (DecalComp ? DecalComp->DecalSize.X : 32.f);
    const float Eps = 0.5f;
    const FVector Loc = Hit.ImpactPoint + N * (Depth * 0.5f - Eps);
    SetActorLocation(Loc);

    // 부착
    if (bAttachToHitComponent && Hit.GetComponent())
    {
        AttachToComponent(Hit.GetComponent(), FAttachmentTransformRules::KeepWorldTransform);
        AttachedComp = Hit.GetComponent();
    }
}
