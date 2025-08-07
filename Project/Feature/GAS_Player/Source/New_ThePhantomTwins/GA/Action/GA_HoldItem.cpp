// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_HoldItem.h"

#include "Player/PlayerCharacter.h"

UGA_HoldItem::UGA_HoldItem()
{
}

void UGA_HoldItem::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	APlayerCharacter* Character = Cast<APlayerCharacter>(ActorInfo->AvatarActor.Get());
	if (!Character) return;

	USkeletalMeshComponent* MeshComp = Character->GetMesh();
	FName HandSocketName = TEXT("Hand_RSocket"); //  // 손 소켓을 설정해야함.  ////

	// 캐릭터가 선택한 아이템을 받아와야함.
	// 손에 아이템을 붙이기.
	// 손에서 충돌판정이 안되도록 하기.

	// UGA_HoldItemㅡ 이 GA에 해당하는 태그가 붙어있는 상태면 손에 뭘 집고 있는 상태의 애니메이션이 출력되도록 하기.

}
