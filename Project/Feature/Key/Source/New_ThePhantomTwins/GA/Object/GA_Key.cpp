#include "GA/Object/GA_Key.h"
#include "Player/PlayerCharacter.h"
#include "Player/FocusTraceComponent.h"
#include "Objects/ConsoleObject.h"
#include "Tags/TPTGameplayTags.h"
#include "Log/TPTLog.h"


UGA_Key::UGA_Key()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	AbilityTags.AddTag(FTPTGameplayTags::Get().TPTGameplay_Character_Object_Key);
}

void UGA_Key::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	TPT_LOG(GALog, Log, TEXT("UGA_Key :: ActivateAbility()"));

	Character = Cast<APlayerCharacter>(ActorInfo->AvatarActor.Get());
	NULLCHECK_CODE_RETURN_LOG(Character, GALog, Warning, EndAbility(Handle, ActorInfo, ActivationInfo, true, false);, );

	TargetActor = Cast<AActor>(Character->GetFocusTrace()->GetFocusedActor());
	NULLCHECK_CODE_RETURN_LOG(TargetActor, GALog, Warning, EndAbility(Handle, ActorInfo, ActivationInfo, true, false);, );

	// TargetActor 정보 출력
	if (TargetActor)
	{
		TPT_LOG(GALog, Log, TEXT("TargetActor Name: %s, Class: %s"), *TargetActor->GetName(), *TargetActor->GetClass()->GetName());
	}


	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
