// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_AimItem.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Log/TPTLog.h"
#include "Tags/TPTGameplayTags.h"
#include "Components/DecalComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/ProjectileMovementComponent.h"

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

	//
	//if (AActor* Avatar = GetAvatarActorFromActorInfo())
	//{
	//	AimDecal = NewObject<UDecalComponent>(Avatar);
	//	AimDecal->RegisterComponent();
	//	AimDecal->SetDecalMaterial(DecalMaterial);
	//	AimDecal->DecalSize = FVector(IndicatorRadius, IndicatorRadius, 200.f);
	//	AimDecal->SetRelativeRotation(FRotator(-90.f, 0.f, 0.f)); // ¹Ù´Ú¿¡ ´¯È÷±â
	//}
}

void UGA_AimItem::UpdateAim()
{
	//if (!AimDecal) return;
	//
	//AActor* Avatar = GetAvatarActorFromActorInfo();
	//if (!Avatar) return;
	//
	//FVector Start = Avatar->GetActorLocation() + Avatar->GetActorForwardVector() * 100.f;
	//FVector LaunchVelocity = Avatar->GetActorForwardVector() * 30.f;
	//
	//FPredictProjectilePathParams PathParams;
	//PathParams.StartLocation = Start;
	//PathParams.LaunchVelocity = LaunchVelocity;
	//PathParams.OverrideGravityZ = GetWorld()->GetGravityZ();
	//PathParams.ProjectileRadius = 5.f;
	//PathParams.SimFrequency = 15.f;
	//PathParams.MaxSimTime = 3.f;
	//PathParams.TraceChannel = ECC_Visibility;
	//
	//FPredictProjectilePathResult PathResult;
	//if (UGameplayStatics::PredictProjectilePath(this, PathParams, PathResult))
	//{
	//	AimDecal->SetWorldLocation(PathResult.HitResult.Location);
	//}
}

void UGA_AimItem::EndAbility(const FGameplayAbilitySpecHandle Handle,
    const FGameplayAbilityActorInfo* ActorInfo,
    const FGameplayAbilityActivationInfo ActivationInfo,
    bool bReplicateEndAbility, bool bWasCancelled)
{
    if (AimDecal)
    {
        AimDecal->DestroyComponent();
        AimDecal = nullptr;
    }

    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}