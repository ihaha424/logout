// Fill out your copyright notice in the Description page of Project Settings.


#include "BoxObject.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"

ABoxObject::ABoxObject() : AInteractableObject()
{

}

void ABoxObject::BeginPlay()
{
	Super::BeginPlay();
}

void ABoxObject::OnInteractServer_Implementation(const APawn* Interactor)
{
	if (bIsActived) return;

	InvokeGameplayCue(Interactor);			// 자기 자신 이펙트 재생
	ApplyEffectToTarget(Interactor);		// 상대방한테 게임플레이 이펙트 발동 시킴
}

void ABoxObject::ApplyEffectToTarget(const APawn* Interactor)
{
	AActor* TargetActor = const_cast<APawn*>(Interactor);

	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);

	if (TargetASC)
	{
		FGameplayEffectContextHandle EffectContext = TargetASC->MakeEffectContext();
		EffectContext.AddSourceObject(this);

		FGameplayEffectSpecHandle EffectSpecHandle = TargetASC->MakeOutgoingSpec(GameplayEffectClass, 1, EffectContext);
		if (EffectSpecHandle.IsValid())
		{
			TargetASC->BP_ApplyGameplayEffectSpecToSelf(EffectSpecHandle);
		}
	}
}

void ABoxObject::InvokeGameplayCue(const APawn* Interactor)
{
	if (!Interactor)
		return;

	AActor* TargetActor = const_cast<APawn*>(Interactor);

	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);

	if (TargetASC)
	{
		FGameplayCueParameters Param;
		Param.SourceObject = this;
		Param.Instigator = TargetActor;
		Param.Location = GetActorLocation();
		TargetASC->ExecuteGameplayCue(GameplayCueTag, Param);
	}
}

