// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_AimItem.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Log/TPTLog.h"
#include "Tags/TPTGameplayTags.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/RepLayout.h"

UGA_AimItem::UGA_AimItem()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;
	AbilityTags.AddTag(FTPTGameplayTags::Get().TPTGameplay_Character_State_AimItem);
}

void UGA_AimItem::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                  const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	TPT_LOG(GALog, Error, TEXT(""));
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	PlayHoldingItemMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("HoldingItemMontage"), HoldingItemMontage, 1.0f);
	PlayHoldingItemMontageTask->ReadyForActivation();

    OwnerActor = ActorInfo->AvatarActor.Get();
    NULLCHECK_CODE_RETURN_LOG(OwnerActor, GALog, Warning, EndAbility(Handle, ActorInfo, ActivationInfo, false, false);, )

    // Spline 컴포넌트 생성 후 플레이어에 붙임
	SplineComp = NewObject<USplineComponent>(OwnerActor, USplineComponent::StaticClass(), TEXT("AimSpline"));
    // 플레이어로부터 메쉬 컴포넌트 찾기
	OwnerMeshComp = OwnerActor->FindComponentByClass<USkeletalMeshComponent>();
    NULLCHECK_CODE_RETURN_LOG(OwnerMeshComp, GALog, Warning, EndAbility(Handle, ActorInfo, ActivationInfo, false, false); , )

    // 부모로 설정
	SplineComp->SetupAttachment(OwnerActor->GetRootComponent());
    SplineComp->RegisterComponent();

	// 주기적 위치 업데이트
	GetWorld()->GetTimerManager().SetTimer(
		UpdateTimerHandle,
		this,
		&UGA_AimItem::UpdateParabola,
		0.5f,
		true
	);
}

void UGA_AimItem::UpdateParabola()
{
    NULLCHECK_CODE_RETURN_LOG(OwnerActor, GALog, Warning, EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false); , )
    NULLCHECK_CODE_RETURN_LOG(OwnerMeshComp, GALog, Warning, EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);, )
    NULLCHECK_CODE_RETURN_LOG(SplineStaticMesh, GALog, Warning, EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);, )
    NULLCHECK_CODE_RETURN_LOG(SplineMaterial, GALog, Warning, EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);, )

    // 기존 스플라인 & 메쉬 초기화
    SplineComp->ClearSplinePoints(false);
    for (auto* Mesh : SplineMeshes)
    {
        if (Mesh) Mesh->DestroyComponent();
    }
    SplineMeshes.Empty();

    // 시작 위치 & 발사 방향
	FVector StartLocation = OwnerMeshComp->GetSocketLocation(TEXT("RightHand")) + FVector{0, -10.f, 0};
    //TPT_LOG(GALog, Warning, TEXT("%d, %d, %d"), StartLocation.X, StartLocation.Y, StartLocation.Z);
    FVector ForwardVector = OwnerActor->GetActorForwardVector() * 1000.f;

    // 포물선 설정
    FPredictProjectilePathParams Params;
    Params.StartLocation = StartLocation;
    Params.LaunchVelocity = ForwardVector;
    Params.bTraceWithCollision = true;
    Params.ProjectileRadius = 5.f;
    Params.MaxSimTime = 2.f;
    Params.SimFrequency = 15.f;
    Params.TraceChannel = ECollisionChannel::ECC_Visibility;
    Params.ActorsToIgnore.Add(OwnerActor);

    FPredictProjectilePathResult Result;
    bool bHit = UGameplayStatics::PredictProjectilePath(OwnerActor, Params, Result);

    // 스플라인 포인트 추가
    for (const FPredictProjectilePathPointData& PointData : Result.PathData)
    {
        SplineComp->AddSplinePoint(PointData.Location, ESplineCoordinateSpace::World, false);
    }
    SplineComp->UpdateSpline();

    // 스플라인 메쉬 생성
    int32 NumSegments = SplineComp->GetNumberOfSplinePoints() - 1;

    for (int32 i = 0; i < NumSegments; i++)
    {
        FVector StartPos = SplineComp->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::World);
        FVector StartTan = SplineComp->GetTangentAtSplinePoint(i, ESplineCoordinateSpace::World);
        FVector EndPos = SplineComp->GetLocationAtSplinePoint(i + 1, ESplineCoordinateSpace::World);
        FVector EndTan = SplineComp->GetTangentAtSplinePoint(i + 1, ESplineCoordinateSpace::World);

        USplineMeshComponent* NewMesh = NewObject<USplineMeshComponent>(OwnerActor);
        if (!NewMesh) continue;

        // 기본 세팅
        NewMesh->SetMobility(EComponentMobility::Movable);
        NewMesh->SetForwardAxis(ESplineMeshAxis::Z);
        NewMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        NewMesh->SetStartScale(FVector2D(0.03f, 0.03f));
        NewMesh->SetEndScale(FVector2D(0.03f, 0.03f));
        NewMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        NewMesh->SetCastShadow(false);
        NewMesh->bCastDynamicShadow = false;
        NewMesh->bCastStaticShadow = false;
    	NewMesh->SetStaticMesh(SplineStaticMesh);

        // 스플라인에 붙이기
        NewMesh->AttachToComponent(SplineComp, FAttachmentTransformRules::KeepWorldTransform);
        NewMesh->RegisterComponent();

        // Start-End 적용
        NewMesh->SetStartAndEnd(StartPos, StartTan, EndPos, EndTan, true);

        // 머테리얼은 반드시 마지막에
    	NewMesh->SetMaterial(0, SplineMaterial);

        SplineMeshes.Add(NewMesh);
    }
}

void UGA_AimItem::EndAbility(const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    bool bReplicateEndAbility, bool bWasCancelled)
{
	GetWorld()->GetTimerManager().ClearTimer(UpdateTimerHandle);

    NULLCHECK_CODE_RETURN_LOG(SplineComp, GALog, Warning, EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false); , );

	SplineComp->ClearSplinePoints(true);

    if (SplineMeshes.Num() > 0)
    {
        for (int32 i = 0; i < SplineMeshes.Num(); i++)
        {
            if (SplineMeshes[i])
            {
                SplineMeshes[i]->DestroyComponent();
            }
        }

        SplineMeshes.Empty();
    }

	if (SplineComp)
	{
        SplineComp->DestroyComponent();
        SplineComp = nullptr;
	}

    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}