#include "GA_ThrowItem.h"
#include "Objects/ThrowNoiseBomb.h"
#include "Objects/ThrowEMP.h"
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
#include "Components/SphereComponent.h"

UGA_ThrowItem::UGA_ThrowItem()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;
}

void UGA_ThrowItem::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	CancelAimItemAbility();
	UAbilityTask_PlayMontageAndWait* PlayThrowItemMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("ThrowItemMontage"), ThrowItemMontage, 1.0f);
	PlayThrowItemMontageTask->OnCompleted.AddDynamic(this, &ThisClass::OnMontageComplete);
	PlayThrowItemMontageTask->OnCancelled.AddDynamic(this, &ThisClass::OnMontageComplete);
	PlayThrowItemMontageTask->OnInterrupted.AddDynamic(this, &ThisClass::OnMontageComplete);

	PlayThrowItemMontageTask->ReadyForActivation();

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
	AActor* OwnerActor = GetAvatarActorFromActorInfo();
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

	// 방향
	FVector Velocity = OwnerActor->GetActorForwardVector() * ThrowItemData->ThrowDistance;

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

			// 속도/스피드 설정
			ProjectileMovementComponent->Velocity = Velocity;
			//const float SpeedMag = InitialVelocity.Size();
			//if (SpeedMag > KINDA_SMALL_NUMBER)
			//{
			//	ProjectileMovementComponent->InitialSpeed = SpeedMag;
			//	ProjectileMovementComponent->MaxSpeed = FMath::Max(ProjectileMovementComponent->MaxSpeed, SpeedMag);
			//}

			ProjectileMovementComponent->SetActive(true);
			ProjectileMovementComponent->Activate(true);
			ProjectileMovementComponent->UpdateComponentVelocity();

			return true;
		};

	// 실제 스폰: NoiseBomb / EMP
	switch (ItemType)
	{
	case EItemType::NoiseBomb:
	{
		AThrowNoiseBomb* ThrowActor = World->SpawnActor<AThrowNoiseBomb>(NoiseBombClass, SpawnLocation, FRotator::ZeroRotator, SpawnParams);
		if (!SetupProjectile(ThrowActor))
		{
			TPT_LOG(GALog, Warning, TEXT("Failed to spawn or setup NoiseBomb actor."));
		}
		ThrowActor->CollisionComponent->IgnoreActorWhenMoving(OwnerActor, true);
		break;
	}
	case EItemType::EMP:
	{
		AThrowEMP* ThrowActor = World->SpawnActor<AThrowEMP>(EMPClass, SpawnLocation, FRotator::ZeroRotator, SpawnParams);
		if (!SetupProjectile(ThrowActor))
		{
			TPT_LOG(GALog, Warning, TEXT("Failed to spawn or setup EMP actor."));
		}
		ThrowActor->CollisionComponent->IgnoreActorWhenMoving(OwnerActor, true);
		break;
	}
	default:
		break;
	}
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
	AActor* OwnerActor = GetAvatarActorFromActorInfo();
	if (!OwnerActor)
		return FRotator::ZeroRotator;

	// OwnerActor가 PlayerState인 경우 Pawn 가져오기
	const APlayerCharacter* PlayerChar = Cast<APlayerCharacter>(OwnerActor);

	if (PlayerChar)
	{
		UCameraComponent* CameraComp = PlayerChar->FindComponentByClass<UCameraComponent>();

		if (CameraComp)
		{// 카메라의 회전값 반환
			return CameraComp->GetComponentRotation();
		}
	}

	// 카메라를 못 찾았으면 컨트롤러 회전 사용
	if (PlayerChar->GetController())
	{
		return PlayerChar->GetController()->GetControlRotation();
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
		//TPT_LOG(GALog, Log, TEXT("Applied custom gravity (DT %f) => ProjectileGravityScale %f"), ThrowItemData->OverrideGravityZ, ProjectileComp->ProjectileGravityScale);
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