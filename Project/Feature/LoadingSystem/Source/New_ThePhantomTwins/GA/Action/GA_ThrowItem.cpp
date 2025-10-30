#include "GA_ThrowItem.h"
#include "Objects/ThrowNoiseBomb.h"
#include "Objects/ThrowEMP.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Abilities/GameplayAbility.h"
#include "Player/PlayerCharacter.h"
#include "Player/PS_Player.h"
#include "Components/SkeletalMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "Tags/TPTGameplayTags.h"
#include "Log/TPTLog.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"

UGA_ThrowItem::UGA_ThrowItem()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;
}

void UGA_ThrowItem::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                    const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	CancelAimItemAbility();

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (ASC)
	{
		FGameplayTagContainer CancelTags;
		CancelTags.AddTag(FTPTGameplayTags::Get().TPTGameplay_Character_State_AimItem);
		ASC->CancelAbilities(&CancelTags);
	}

	if (HasAuthority(&ActivationInfo))
	{
		EItemType ItemType = EItemType::None;
		if (TriggerEventData)
		{
			ItemType = static_cast<EItemType>((int32)TriggerEventData->EventMagnitude);
		}

		SpawnThrowableItem(ItemType);
	}
	UAbilityTask_PlayMontageAndWait* PlayThrowItemMontageeTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("ThrowItemMontage"), ThrowItemMontage, 1.0f);
	PlayThrowItemMontageeTask->OnCompleted.AddDynamic(this, &ThisClass::OnMontageComplete);
	PlayThrowItemMontageeTask->OnInterrupted.AddDynamic(this, &ThisClass::OnMontageComplete);

	PlayThrowItemMontageeTask->ReadyForActivation();
}

void UGA_ThrowItem::SpawnThrowableItem(EItemType ItemType)
{
	// 필수 클래스 체크
	if (!NoiseBombClass || !EMPClass)
	{
		TPT_LOG(GALog, Warning, TEXT("Required classes (NoiseBombClass or EMPClass) are not set!"));
		return;
	}

	// 소유자 (OwnerActor는 보통 PlayerState로 설정되어 있을 수 있음)
	AActor* OwnerActor = GetOwningActorFromActorInfo();
	if (!OwnerActor)
	{
		TPT_LOG(GALog, Warning, TEXT("UGA_ThrowItem: No owning actor."));
		return;
	}

	// DataTable row
	FThrowItemDT* ThrowItemData = GetThrowItemData(ItemType);
	if (!ThrowItemData)
	{
		TPT_LOG(GALog, Warning, TEXT("ThrowItemData not found for ItemType"));
		return;
	}

	// 시작 위치: Aim과 동일하게 RightHandSocket + DT StartOffset 사용
	FVector SpawnLocation = GetRightHandSocketLocation() + ThrowItemData->StartOffset;
	if (SpawnLocation.IsZero())
	{
		SpawnLocation = OwnerActor->GetActorLocation() + ThrowItemData->StartOffset;
	}

	// 목표 위치 계산 (카메라/컨트롤러/actor 순)
	FVector TargetLocation = CalculateTargetLocation(ThrowItemData, SpawnLocation);

	// SpawnRotation: 카메라 우선(UGA_AimItem과 동일)
	FRotator SpawnRotation = GetThrowRotation(SpawnLocation, TargetLocation);

	// LaunchVelocity 결정
	FVector LaunchVelocity = FVector::ZeroVector;
	bool bHaveVelocity = false;

	// 1) DT에 LaunchVelocity가 있으면 그것을 '로컬(SpawnRotation 기준)' 벡터로 해석해서 월드속도로 변환
	if (!ThrowItemData->LaunchVelocity.IsNearlyZero())
	{
		SpawnRotation = GetThrowRotation(SpawnLocation, TargetLocation);
		LaunchVelocity = SpawnRotation.RotateVector(ThrowItemData->LaunchVelocity); // DT 값은 로컬 기준 저장 가정
		bHaveVelocity = true;

		TPT_LOG(GALog, Log, TEXT("UGA_ThrowItem: Using DT LaunchVelocity (local->world): %s"), *LaunchVelocity.ToString());
	}
	else
	{
		// 2) DT에 LaunchVelocity가 없으면 '고정 발사 각도' 방식으로 속도 계산 (카메라 피치 무시 — 수평 거리 고정)
		UWorld* World = GetWorld();
		if (World)
		{
			// 목표 수평 거리: DT의 ThrowDistance 우선, 없으면 Spawn->Target 수평 거리 사용
			float DesiredRange = ThrowItemData->ThrowDistance;
			if (FMath::IsNearlyZero(DesiredRange))
			{
				FVector Delta = TargetLocation - SpawnLocation;
				Delta.Z = 0.f;
				DesiredRange = Delta.Size();
				if (FMath::IsNearlyZero(DesiredRange))
				{
					DesiredRange = 1000.f; // 안전 fallback
				}
			}

			// 중력 값 결정 (DT의 OverrideGravityZ가 있으면 사용, 없으면 월드 중력)
			float GravityZ = 0.f;
			if (!FMath::IsNearlyZero(ThrowItemData->OverrideGravityZ))
			{
				GravityZ = ThrowItemData->OverrideGravityZ;
			}
			else
			{
				GravityZ = World ? World->GetGravityZ() : -980.f;
			}
			float g = FMath::Abs(GravityZ);
			if (g <= KINDA_SMALL_NUMBER)
			{
				g = 980.f; // 안전 fallback
			}

			// 각도 -> 라디안
			float ThetaDeg = FixedLaunchAngleDegrees;
			float ThetaRad = FMath::DegreesToRadians(ThetaDeg);

			// sin(2θ) 검증
			float Sin2Theta = FMath::Sin(2.f * ThetaRad);
			if (FMath::Abs(Sin2Theta) < KINDA_SMALL_NUMBER)
			{
				// 각도가 불안정하면 fallback 사용
				const float FallbackSpeed = 1000.f;
				SpawnRotation = GetThrowRotation(SpawnLocation, TargetLocation);
				LaunchVelocity = SpawnRotation.Vector() * FallbackSpeed;
				bHaveVelocity = true;
				TPT_LOG(GALog, Warning, TEXT("UGA_ThrowItem: Fixed-angle calc unstable (sin2θ ~ 0). Using fallback velocity: %s"), *LaunchVelocity.ToString());
			}
			else
			{
				// 초기 속도 크기 계산: v = sqrt(R * g / sin(2θ))
				float RequiredSpeed = FMath::Sqrt(FMath::Max(0.f, DesiredRange * g / Sin2Theta));

				// 수평 방향(방위각)만 사용 -> 카메라 피치 무시
				FVector ToTarget = TargetLocation - SpawnLocation;
				FVector DirXY = ToTarget;
				DirXY.Z = 0.f;
				if (DirXY.IsNearlyZero())
				{
					// 방향을 못구하면 액터의 forward 사용
					DirXY = OwnerActor ? OwnerActor->GetActorForwardVector() : FVector::ForwardVector;
					DirXY.Z = 0.f;
				}
				DirXY = DirXY.GetSafeNormal();

				// Launch vector 구성: (cosθ * horizontal) + (sinθ * up) scaled by speed
				float CosTheta = FMath::Cos(ThetaRad);
				float SinTheta = FMath::Sin(ThetaRad);

				LaunchVelocity = DirXY * (CosTheta * RequiredSpeed) + FVector::UpVector * (SinTheta * RequiredSpeed);

				// SpawnRotation도 발사 방향으로 맞춤
				SpawnRotation = LaunchVelocity.Rotation();

				bHaveVelocity = true;

				TPT_LOG(GALog, Log, TEXT("UGA_ThrowItem: Fixed-angle LaunchVelocity calculated. Angle %f deg, Speed %f, Velocity %s"), ThetaDeg, RequiredSpeed, *LaunchVelocity.ToString());
			}
		}
		else
		{
			// 월드가 없으면 fallback
			const float FallbackSpeed = 1000.f;
			LaunchVelocity = SpawnRotation.Vector() * FallbackSpeed;
			bHaveVelocity = true;
			TPT_LOG(GALog, Warning, TEXT("UGA_ThrowItem: World missing, using fallback LaunchVelocity: %s"), *LaunchVelocity.ToString());
		}
	}

	// Spawn 파라미터
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = OwnerActor;
	SpawnParams.Instigator = Cast<APawn>(OwnerActor);
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	UWorld* World = GetWorld();
	if (!World) return;

	// lambda로 설정 적용
	auto SetupProjectile = [&](AActor* ThrowActor) -> bool
		{
			if (!ThrowActor) return false;

			UProjectileMovementComponent* ProjectileMovementComponent = nullptr;
			if (ItemType == EItemType::NoiseBomb)
			{
				AThrowNoiseBomb* NoiseBomb = Cast<AThrowNoiseBomb>(ThrowActor);
				ProjectileMovementComponent = NoiseBomb ? NoiseBomb->ProjectileMovementComponent : nullptr;
			}
			else if (ItemType == EItemType::EMP)
			{
				AThrowEMP* EMP = Cast<AThrowEMP>(ThrowActor);
				ProjectileMovementComponent = EMP ? EMP->ProjectileMovementComponent : nullptr;
			}

			if (!ProjectileMovementComponent) return false;

			// DT에 정의된 설정 적용 (중력 스케일 등)
			ApplyThrowItemDataSettings(ProjectileMovementComponent, ThrowItemData);

			// 초기 속도 적용
			FVector InitialVelocity = LaunchVelocity;
			if (!bHaveVelocity || InitialVelocity.IsNearlyZero())
			{
				InitialVelocity = SpawnRotation.Vector() * ProjectileMovementComponent->InitialSpeed;
			}

			// 속도/스피드 설정
			ProjectileMovementComponent->Velocity = InitialVelocity;
			const float SpeedMag = InitialVelocity.Size();
			if (SpeedMag > KINDA_SMALL_NUMBER)
			{
				ProjectileMovementComponent->InitialSpeed = SpeedMag;
				ProjectileMovementComponent->MaxSpeed = FMath::Max(ProjectileMovementComponent->MaxSpeed, SpeedMag);
			}

			ProjectileMovementComponent->SetActive(true);
			ProjectileMovementComponent->Activate(true);
			ProjectileMovementComponent->UpdateComponentVelocity();

			// 투사체 액터의 회전도 발사 방향으로 맞춤(모델 축에 따라 보정 필요시 추가)
			ThrowActor->SetActorRotation(InitialVelocity.Rotation());

			return true;
		};

	// 실제 스폰: NoiseBomb / EMP
	switch (ItemType)
	{
	case EItemType::NoiseBomb:
	{
		AThrowNoiseBomb* ThrowActor = World->SpawnActor<AThrowNoiseBomb>(NoiseBombClass, SpawnLocation, SpawnRotation, SpawnParams);
		if (!SetupProjectile(ThrowActor))
		{
			TPT_LOG(GALog, Warning, TEXT("Failed to spawn or setup NoiseBomb actor."));
		}
		break;
	}
	case EItemType::EMP:
	{
		AThrowEMP* ThrowActor = World->SpawnActor<AThrowEMP>(EMPClass, SpawnLocation, SpawnRotation, SpawnParams);
		if (!SetupProjectile(ThrowActor))
		{
			TPT_LOG(GALog, Warning, TEXT("Failed to spawn or setup EMP actor."));
		}
		break;
	}
	default:
		break;
	}
}


FVector UGA_ThrowItem::CalculateTargetLocation(const FThrowItemDT* ThrowItemData, const FVector& StartLocation) const
{
	AActor* OwnerActor = GetOwningActorFromActorInfo();
	if (!OwnerActor || !ThrowItemData)
	{
		return StartLocation + FVector::ForwardVector * (ThrowItemData ? ThrowItemData->ThrowDistance : 1000.0f);
	}

	FVector ViewDir = FVector::ZeroVector;

	// OwnerActor가 PlayerState인 경우 Pawn 가져오기
	APS_Player* PlayerState = Cast<APS_Player>(OwnerActor);
	if (PlayerState)
	{
		APawn* Pawn = PlayerState->GetPawn();
		if (Pawn)
		{
			// PlayerCharacter로 캐스트해서 카메라 찾기
			const APlayerCharacter* PlayerChar = Cast<APlayerCharacter>(Pawn);
			if (PlayerChar)
			{
				UCameraComponent* CameraComp = PlayerChar->FindComponentByClass<UCameraComponent>();
				if (CameraComp)
				{
					ViewDir = CameraComp->GetForwardVector();
				}
			}
		}
	}

	// 카메라를 못 찾았으면 컨트롤러 회전 사용 (기존 방식)
	if (ViewDir.IsNearlyZero())
	{
		if (PlayerState)
		{
			APawn* Pawn = PlayerState->GetPawn();
			if (Pawn && Pawn->GetController())
			{
				FRotator ControlRot = Pawn->GetController()->GetControlRotation();
				ViewDir = ControlRot.Vector();
			}
		}
	}

	// 그것도 안 되면 액터 Forward 방향
	if (ViewDir.IsNearlyZero())
	{
		ViewDir = OwnerActor->GetActorForwardVector();
	}

	return StartLocation + ViewDir.GetSafeNormal() * ThrowItemData->ThrowDistance;
}

FVector UGA_ThrowItem::GetRightHandSocketLocation() const
{
	AActor* OwnerAvatar = GetAvatarActorFromActorInfo();
	if (!OwnerAvatar) return FVector::ZeroVector;

	const APlayerCharacter* PlayerChar = Cast<APlayerCharacter>(OwnerAvatar);
	if (PlayerChar)
	{
		if (const USkeletalMeshComponent* MeshComp = PlayerChar->GetMesh())
		{
			if (MeshComp->DoesSocketExist(TEXT("RightHandSocket")))
			{
				return MeshComp->GetSocketLocation(TEXT("RightHandSocket"));
			}
		}
	}
	return FVector::ZeroVector;
}

FRotator UGA_ThrowItem::GetThrowRotation(const FVector& StartLocation, const FVector& TargetLocation) const
{
	AActor* OwnerActor = GetOwningActorFromActorInfo();
	if (!OwnerActor)
		return FRotator::ZeroRotator;

	// OwnerActor가 PlayerState인 경우 Pawn 가져오기
	APS_Player* PlayerState = Cast<APS_Player>(OwnerActor);
	if (PlayerState)
	{
		APawn* Pawn = PlayerState->GetPawn();
		if (Pawn)
		{
			const APlayerCharacter* PlayerChar = Cast<APlayerCharacter>(Pawn);
			if (PlayerChar)
			{
				UCameraComponent* CameraComp = PlayerChar->FindComponentByClass<UCameraComponent>();
				if (CameraComp)
				{
					// 카메라의 회전값 반환
					return CameraComp->GetComponentRotation();
				}
			}
			// 카메라를 못 찾았으면 컨트롤러 회전 사용
			if (Pawn->GetController())
			{
				return Pawn->GetController()->GetControlRotation();
			}
		}
	}
	// 컨트롤러가 없으면 액터가 바라보는 방향으로 기본 반환
	return OwnerActor->GetActorRotation();
}

void UGA_ThrowItem::InitializeProjectileMovement(UProjectileMovementComponent* ProjectileMovementComponent, const FVector& LaunchVelocity, const FRotator& SpawnRotation) const
{
	if (!ProjectileMovementComponent) return;
	ProjectileMovementComponent->Velocity = LaunchVelocity;
	ProjectileMovementComponent->SetActive(true);
	ProjectileMovementComponent->Activate(true);
	ProjectileMovementComponent->UpdateComponentVelocity();
}

void UGA_ThrowItem::ApplyThrowItemDataSettings(UProjectileMovementComponent* ProjectileComp, const FThrowItemDT* ThrowItemData) const
{
	if (!ProjectileComp || !ThrowItemData) return;

	// DT의 OverrideGravityZ을 '실제 중력값'(-980 등)으로 넣는 것을 전제로 gravity scale 계산
	if (!FMath::IsNearlyZero(ThrowItemData->OverrideGravityZ))
	{
		UWorld* World = GetWorld();
		float WorldGravityZ = World ? World->GetGravityZ() : -980.0f;
		if (FMath::IsNearlyZero(WorldGravityZ))
		{
			// 안전 fallback (월드 중력이 0이면 scale을 1로 둠)
			ProjectileComp->ProjectileGravityScale = 1.0f;
		}
		else
		{
			// 비율 계산: (DT가 -490, World가 -980 => scale 0.5)
			float GravityScale = ThrowItemData->OverrideGravityZ / WorldGravityZ;
			ProjectileComp->ProjectileGravityScale = GravityScale;
		}
		TPT_LOG(GALog, Log, TEXT("Applied custom gravity (DT %f) => ProjectileGravityScale %f"), ThrowItemData->OverrideGravityZ, ProjectileComp->ProjectileGravityScale);
	}

	// 추가 설정(바운스, 중첩 등)은 필요 시 ThrowItemDT에 필드를 추가하고 여기서 적용
}

FThrowItemDT* UGA_ThrowItem::GetThrowItemData(EItemType ItemType) const
{
	if (!ThrowItemDT) return nullptr;

	FName RowName = NAME_None;
	switch (ItemType)
	{
	case EItemType::NoiseBomb:
		RowName = FName(TEXT("NoiseBomb"));
		break;
	case EItemType::EMP:
		RowName = FName(TEXT("EMP"));
		break;
	default:
		return nullptr;
	}

	static const FString ContextString(TEXT("UGA_ThrowItem::GetThrowItemData"));
	return ThrowItemDT->FindRow<FThrowItemDT>(RowName, ContextString, true);
}

void UGA_ThrowItem::CancelAimItemAbility()
{
	// AbilitySystemComponent 가져오기
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC)
	{
		TPT_LOG(GALog, Warning, TEXT("UGA_ThrowItem: AbilitySystemComponent not found"));
		return;
	}

	// 방법 1: 태그로 어빌리티 취소 (권장)
	FGameplayTagContainer TagsToCancel;
	TagsToCancel.AddTag(FTPTGameplayTags::Get().TPTGameplay_Character_State_AimItem);
	ASC->CancelAbilities(&TagsToCancel);
}

void UGA_ThrowItem::OnMontageComplete()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);

}