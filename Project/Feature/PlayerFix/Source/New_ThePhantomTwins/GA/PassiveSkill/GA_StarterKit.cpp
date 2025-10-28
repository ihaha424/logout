// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_StarterKit.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Player/PlayerCharacter.h"
#include "Player/PS_Player.h"
#include "Log/TPTLog.h"
#include "Objects/InventoryComponent.h"

UGA_StarterKit::UGA_StarterKit()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor; 
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

void UGA_StarterKit::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// ¿Œ∫•≈‰∏Æ »Æ¿Œ
	const APlayerCharacter* Character = Cast<APlayerCharacter>(ActorInfo->AvatarActor);
	NULLCHECK_CODE_RETURN_LOG(Character, GALog, Warning, EndAbility(Handle, ActorInfo, ActivationInfo, true, false);,);

	const APS_Player* PS = Cast<APS_Player>(Character->GetPlayerState());
	NULLCHECK_CODE_RETURN_LOG(PS, GALog, Warning, EndAbility(Handle, ActorInfo, ActivationInfo, true, false); , );
	UInventoryComponent* Inventory = PS->InventoryComp;
	NULLCHECK_CODE_RETURN_LOG(Inventory, GALog, Warning, EndAbility(Handle, ActorInfo, ActivationInfo, true, false); , );

	EItemType ItemType = EItemType::None;
	if (TriggerEventData)
	{
		ItemType = static_cast<EItemType>((int32)TriggerEventData->EventMagnitude);
	}

	TPT_LOG(GALog, Log, TEXT("Item Number : %d"), static_cast<int32>(ItemType));

	switch (ItemType)
	{
	case EItemType::NoiseBomb:
	case EItemType::EMP:
	case EItemType::Key:	
		Inventory->AddItem(ItemType);
		break;
	case EItemType::HealPack:	// »˙∆—
	case EItemType::MentalPack:	// ∏‡≈ª∆—
		for (int32 i = 0; i < 2; i++)
		{
			Inventory->AddItem(ItemType);
		}
		break;
	default:
		break;
	}

	//// ∑£¥˝ Enum √£±‚
	//int32 RandomNumber = FMath::RandRange(1, 6);

	//// ¿Œ∫•≈‰∏Æø° ≈€ ¡§«ÿ¡¯ ∞≥ºˆ∏∏≈≠ ∫Œø©
	//switch (RandomNumber)
	//{
	//case 1:	// ∆¯¡◊
	//case 2:	// ƒ´∏ﬁ∂Û
	//case 5:	// ≈∞
	//	Inventory->AddItem(EItemType(RandomNumber));
	//	break;
	//case 3:	// »˙∆—
	//case 4:	// ∏‡≈ª∆—
	//case 6:	// µÂ∏µ≈©
	//	for (int32 i = 0; i < 2; i++)
	//	{
	//		Inventory->AddItem(EItemType(RandomNumber));
	//	}
	//	break;
	//default:
	//	break;
	//}

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

