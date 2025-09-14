#include "GA_ThrowItem.h"
#include "Objects/ThrowNoiseBomb.h"
#include "Objects/ThrowEMP.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Player/PlayerCharacter.h"
#include "Components/SkeletalMeshComponent.h"
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
		UE_LOG(LogTemp, Warning, TEXT("Required classes (NoiseBombClass or EMPClass) are not set!"));
		return;
	}

	AActor* OwnerActor = GetOwningActorFromActorInfo();
	if (!OwnerActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("UGA_ThrowItem: No owning actor."));
		return;
	}

	FVector SpawnLocation = GetRightHandSocketLocation();
	if (SpawnLocation.IsZero())
	{
		SpawnLocation = OwnerActor->GetActorLocation();
	}

	FVector TargetLocation = CalculateTargetLocation(SpawnLocation);
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

			TPT_LOG(GALog, Warning, TEXT("UGA_ThrowItem :: %s 생성"), *ThrowActor->GetName());

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
			UE_LOG(LogTemp, Warning, TEXT("Failed to spawn or setup NoiseBomb actor."));
		}
		break;
	}
	case EItemType::EMP:
	{
		AThrowEMP* ThrowActor = World->SpawnActor<AThrowEMP>(EMPClass, SpawnLocation, SpawnRotation, SpawnParams);
		if (!SetupProjectile(ThrowActor))
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to spawn or setup EMP actor."));
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
	APawn* Pawn = Cast<APawn>(OwnerActor);
	if (Pawn && Pawn->GetController())
	{
		FRotator ControlRot = Pawn->GetController()->GetControlRotation();
		ViewDir = ControlRot.Vector();
	}

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
	return UKismetMathLibrary::FindLookAtRotation(StartLocation, TargetLocation);
}

void UGA_ThrowItem::InitializeProjectileMovement(UProjectileMovementComponent* ProjectileMovementComponent, const FVector& LaunchVelocity, const FRotator& SpawnRotation) const
{
	if (!ProjectileMovementComponent) return;

	ProjectileMovementComponent->Velocity = LaunchVelocity;
	ProjectileMovementComponent->SetActive(true);
	ProjectileMovementComponent->Activate(true);
	ProjectileMovementComponent->UpdateComponentVelocity();
}