#include "GA_ThrowItem.h"
#include "Objects/ThrowNoiseBomb.h"
#include "Objects/ThrowEMP.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Player/PlayerCharacter.h"
#include "Player/PS_Player.h"
#include "Components/SkeletalMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "Log/TPTLog.h"

UGA_ThrowItem::UGA_ThrowItem()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
}

void UGA_ThrowItem::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	if (HasAuthority(&ActivationInfo))
	{
		EItemType ItemType = EItemType::None;
		if (TriggerEventData)
		{
			ItemType = static_cast<EItemType>((int32)TriggerEventData->EventMagnitude);
		}

		SpawnThrowableItem(ItemType);
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

void UGA_ThrowItem::SpawnThrowableItem(EItemType ItemType)
{
	if (!NoiseBombClass || !EMPClass)
	{
		TPT_LOG(GALog, Warning, TEXT("Required classes (NoiseBombClass or EMPClass) are not set!"));
		return;
	}

	AActor* OwnerActor = GetOwningActorFromActorInfo();
	if (!OwnerActor)
	{
		TPT_LOG(GALog, Warning, TEXT("UGA_ThrowItem: No owning actor."));
		return;
	}

	// FThrowItemDT 통해 던질 아이템 정보 가져오기
	FThrowItemDT* ThrowItemData = GetThrowItemData(ItemType);

	FVector SpawnLocation = GetRightHandSocketLocation() + ThrowItemData->StartOffset;
	if (SpawnLocation.IsZero())
	{
		SpawnLocation = OwnerActor->GetActorLocation();
	}

	// 아이템을 던질 목표 지점을 계산
	FVector TargetLocation = CalculateTargetLocation(SpawnLocation);

	// 플레이어가 바라보고 있는 방향
	FRotator SpawnRotation = GetThrowRotation(SpawnLocation, TargetLocation);

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = OwnerActor;
	SpawnParams.Instigator = Cast<APawn>(OwnerActor);
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	UWorld* World = GetWorld();
	if (!World) return;

	FVector LaunchVelocity;
	bool bHaveVelocity = UGameplayStatics::SuggestProjectileVelocity_CustomArc(
		World,
		LaunchVelocity,
		SpawnLocation,
		TargetLocation,
		0.0f,
		Arc
	);

	auto SetupProjectile = [&](AActor* ThrowActor)
		{
			if (!ThrowActor) return false;

			//TPT_LOG(GALog, Warning, TEXT("UGA_ThrowItem :: %s 생성"), *ThrowActor->GetName());

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

			if (ProjectileMovementComponent)
			{
				InitializeProjectileMovement(ProjectileMovementComponent, bHaveVelocity ? LaunchVelocity : SpawnRotation.Vector() * ProjectileMovementComponent->InitialSpeed, SpawnRotation);
				return true;
			}

			return false;
		};

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

FVector UGA_ThrowItem::CalculateTargetLocation(const FVector& StartLocation) const
{
	AActor* OwnerActor = GetOwningActorFromActorInfo();
	if (!OwnerActor)
		return StartLocation + FVector::ForwardVector * ThrowDistance;

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
				// 카메라 컴포넌트 찾기
				UCameraComponent* CameraComp = PlayerChar->FindComponentByClass<UCameraComponent>();
				if (CameraComp)
				{
					// 카메라가 바라보는 방향 벡터 가져오기
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

	return StartLocation + ViewDir.GetSafeNormal() * ThrowDistance;
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

FThrowItemDT* UGA_ThrowItem::GetThrowItemData(EItemType ItemType) const
{
	if (!ThrowItemDT) return nullptr;

	// Enum 값 → FName으로 변환 (DataTable RowName과 맞춰야 함)
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

	// DataTable에서 row 찾기
	static const FString ContextString(TEXT("UGA_ThrowItem::GetThrowItemData"));
	return ThrowItemDT->FindRow<FThrowItemDT>(RowName, ContextString, true);
}