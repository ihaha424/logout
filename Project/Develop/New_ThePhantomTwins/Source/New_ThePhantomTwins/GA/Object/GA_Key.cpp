#include "GA/Object/GA_Key.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
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

	Character = Cast<APlayerCharacter>(ActorInfo->AvatarActor.Get());
	NULLCHECK_CODE_RETURN_LOG(Character, GALog, Warning, EndAbility(Handle, ActorInfo, ActivationInfo, true, false);, );

	//TargetActor = Cast<AActor>(Character->GetFocusTrace()->GetFocusedActor());
	//NULLCHECK_CODE_RETURN_LOG(TargetActor, GALog, Warning, EndAbility(Handle, ActorInfo, ActivationInfo, true, false);, );

	// TargetActor 정보 출력
	if (TargetActor)
	{
		TPT_LOG(GALog, Log, TEXT("TargetActor Name: %s, Class: %s"), *TargetActor->GetName(), *TargetActor->GetClass()->GetName());
	}

	UAbilityTask_PlayMontageAndWait* PlayDrinkMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("UseKeyMontage"), UseKeyMontage, 1.0f);
	PlayDrinkMontageTask->OnCompleted.AddDynamic(this, &UGA_Key::OnMontageComplete);

	PlayDrinkMontageTask->ReadyForActivation();
}

void UGA_Key::OnMontageComplete()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}