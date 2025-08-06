// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_StarterKit.h"
#include "Player/PlayerCharacter.h"
#include "Player/PS_Player.h"
#include "Log/TPTLog.h"
#include "Objects/InventoryComponent.h"

UGA_StarterKit::UGA_StarterKit()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGA_StarterKit::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// ¿Œ∫•≈‰∏Æ »Æ¿Œ
	const APlayerCharacter* Character = Cast<APlayerCharacter>(ActorInfo->AvatarActor);
	NULLCHECK_RETURN_LOG(Character, GALog, Warning, )
		const APS_Player* PS = Cast<APS_Player>(Character->GetPlayerState());
	NULLCHECK_RETURN_LOG(PS, GALog, Warning, )
		UInventoryComponent* Inventory = PS->InventoryComp;
	NULLCHECK_RETURN_LOG(Inventory, GALog, Warning, )

	// ∑£¥˝ Enum √£±‚
	int32 RandomNumber = FMath::RandRange(1, 6);

	// ¿Œ∫•≈‰∏Æø° ≈€ ¡§«ÿ¡¯ ∞≥ºˆ∏∏≈≠ ∫Œø©
	switch (RandomNumber)
	{
	case 1:	// ∆¯¡◊
	case 2:	// ƒ´∏ﬁ∂Û
	case 5:	// ≈∞
		Inventory->AddItem(EItemType(RandomNumber));
		break;
	case 3:	// »˙∆—
	case 4:	// ∏‡≈ª∆—
	case 6:	// µÂ∏µ≈©
		for (int32 i = 0; i < 2; i++)
		{
			Inventory->AddItem(EItemType(RandomNumber));
		}
		break;
	default:
		break;
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

