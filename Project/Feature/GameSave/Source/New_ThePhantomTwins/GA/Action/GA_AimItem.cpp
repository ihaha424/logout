// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_AimItem.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Log/TPTLog.h"
#include "Tags/TPTGameplayTags.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/RepLayout.h"
#include "Player/PlayerCharacter.h"

UGA_AimItem::UGA_AimItem()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;
	FGameplayTagContainer DefaultTags;
    DefaultTags.AddTag(FTPTGameplayTags::Get().TPTGameplay_Character_State_AimItem);
    SetAssetTags(DefaultTags);
}

void UGA_AimItem::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                  const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	PlayHoldingItemMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("HoldingItemMontage"), HoldingItemMontage, 1.0f);
	PlayHoldingItemMontageTask->ReadyForActivation();
    PlayHoldingItemMontageTask->OnInterrupted.AddDynamic(this, &ThisClass::OnMontageInterrupted);
    // 중간에 방해를 받으면 다시 재생되도록 하기. 커스텀할수있다고는 하다...

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
		0.1f,
		true
	);
}

void UGA_AimItem::UpdateParabola()
{
    NULLCHECK_CODE_RETURN_LOG(OwnerActor, GALog, Warning, EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false); , );
    NULLCHECK_CODE_RETURN_LOG(OwnerMeshComp, GALog, Warning, EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);, );
    NULLCHECK_CODE_RETURN_LOG(SplineStaticMesh, GALog, Warning, EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);, );
    NULLCHECK_CODE_RETURN_LOG(SplineMaterial, GALog, Warning, EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);, );

	APlayerCharacter* Character = Cast<APlayerCharacter>(OwnerActor);
    NULLCHECK_CODE_RETURN_LOG(Character, GALog, Warning, EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false); , );

	if (Character->IsLocallyControlled())
	{
		// 기존 스플라인 & 메쉬 초기화
		SplineComp->ClearSplinePoints(false);
		for (auto* Mesh : SplineMeshes)
		{
			if (Mesh) Mesh->DestroyComponent();
		}
		SplineMeshes.Empty();

		// 시작 위치 & 발사 방향
		StartLocation = OwnerMeshComp->GetSocketLocation(TEXT("RightHand"));
		//TPT_LOG(GALog, Warning, TEXT("%d, %d, %d"), StartLocation.X, StartLocation.Y, StartLocation.Z);
		ForwardVector = OwnerActor->GetActorForwardVector();

		// 포물선 설정
		FPredictProjectilePathParams Params;
		Params.StartLocation = StartLocation + FVector{ 0, -20.f, 0 };                           // 시작 위치
		Params.LaunchVelocity = ForwardVector * 1000.f;                 // 발사 방향과 속도
		Params.bTraceWithCollision = true;                              // 충돌 체크
		Params.ProjectileRadius = 5.f;                                  // 충돌 체크할 때 투사체 반경
		Params.SimFrequency = 1000.f;		                                // 최대 시뮬레이션 시간
		Params.MaxSimTime = 1.f;        	                            // 시뮬레이션 빈도 (몇 초 동안 궤적을 계산할지)
		Params.TraceChannel = ECollisionChannel::ECC_WorldStatic;       // 충돌 검사용 채널
		Params.ActorsToIgnore.Add(OwnerActor);                        // 충돌 무시할 액터 리스트
		//Params.OverrideGravityZ = -980.f;                             // 중력 가속도 (기본값 -980)
		//Params.bTraceComplex = true;                                  // 복잡한 충돌 검사
		//Params.DrawDebugType = EDrawDebugTrace::ForOneFrame;          // 디버그 드로잉 옵션
		//Params.DrawDebugTime = 5.f;                                   // 디버그 드로잉 시간

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
}

void UGA_AimItem::EndAbility(const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    bool bReplicateEndAbility, bool bWasCancelled)
{
	GetWorld()->GetTimerManager().ClearTimer(UpdateTimerHandle);

    NULLCHECK_RETURN_LOG(SplineComp, GALog, Warning, );

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

void UGA_AimItem::OnMontageInterrupted()
{
    PlayHoldingItemMontageTask->ReadyForActivation();
}
