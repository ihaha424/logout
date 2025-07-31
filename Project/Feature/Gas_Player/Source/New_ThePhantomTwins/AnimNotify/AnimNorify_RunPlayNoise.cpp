// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify/AnimNorify_RunPlayNoise.h"
#include "AbilitySystemComponent.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "Tags/TPTGameplayTags.h"
#include "Log/TPTLog.h"
#include "Player/PlayerCharacter.h"

void UAnimNorify_RunPlayNoise::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!MeshComp || !StepSound) return;

	APlayerCharacter* Owner = Cast<APlayerCharacter>(MeshComp->GetOwner());
	NULLCHECK_RETURN_LOG(Owner, GALog, Error, )

	UAbilitySystemComponent* ASC = Owner->GetAbilitySystemComponent();
	NULLCHECK_RETURN_LOG(ASC, GALog, Error, )

	// 달리기
	if (ASC->HasMatchingGameplayTag(FTPTGameplayTags::Get().TPTGameplay_InputTag_Player_Run))
	{
		// 사운드 재생
		UGameplayStatics::PlaySoundAtLocation(Owner, StepSound, Owner->GetActorLocation());

		// 노이즈 발생
		Owner->MakeNoise(1.0f, Owner, Owner->GetActorLocation());
	}
	else // 걷기
	{
		// 사운드 재생
		UGameplayStatics::PlaySoundAtLocation(Owner, StepSound, Owner->GetActorLocation());

		// 노이즈 발생
		Owner->MakeNoise(1.0f, Owner, Owner->GetActorLocation());
	}
}
