// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify/AnimNorify_RunPlayNoise.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "Tags/TPTGameplayTags.h"
#include "Log/TPTLog.h"
#include "Player/PlayerCharacter.h"

void UAnimNorify_RunPlayNoise::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if(!MeshComp || !StepSound) return;

	APlayerCharacter* Owner = Cast<APlayerCharacter>(MeshComp->GetOwner());
	NULLCHECK_RETURN_LOG(Owner, GALog, Error, )

	if (!Owner->IsLocallyControlled()) return;

	UAbilitySystemComponent* ASC = Owner->GetAbilitySystemComponent();
	NULLCHECK_RETURN_LOG(ASC, GALog, Error, )

	if (ASC->HasMatchingGameplayTag(FTPTGameplayTags::Get().TPTGameplay_InputTag_Player_Run))
	{
		// 사운드 재생
		UGameplayStatics::PlaySoundAtLocation(Owner, StepSound, Owner->GetActorLocation());

		// 노이즈 발생
		Owner->MakeNoise(RunNoise, Owner, Owner->GetActorLocation(), 0.f, "PlayerRun");
	}
	else
	{
		// 사운드 재생
		UGameplayStatics::PlaySoundAtLocation(Owner, StepSound, Owner->GetActorLocation());

		// 노이즈 발생
		Owner->MakeNoise(WalkNoise, Owner, Owner->GetActorLocation(), 0.f, "PlayerWalk");
	}
}
