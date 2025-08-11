// Fill out your copyright notice in the Description page of Project Settings.


#include "SzObjects/OverlapObject.h"
#include "Components/BoxComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"

AOverlapObject::AOverlapObject()
{
	BoxTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
	BoxTrigger->SetCollisionProfileName(TEXT("OverlapAll"));
	BoxTrigger->SetGenerateOverlapEvents(true);
	BoxTrigger->SetupAttachment(RootSceneComp);
}

void AOverlapObject::BeginPlay()
{
	Super::BeginPlay();

	if (BoxTrigger)
	{
		BoxTrigger->OnComponentBeginOverlap.AddDynamic(this, &AOverlapObject::OnTriggerBeginOverlap);
		BoxTrigger->OnComponentEndOverlap.AddDynamic(this, &AOverlapObject::OnTriggerEndOverlap);
	}
}

void AOverlapObject::OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor || OtherActor == this)
		return;

	// 이미 적용된 대상이 아니면
	if (!ActiveEffectHandles.Contains(OtherActor))
	{
		// 1) 지속형 GE 적용 및 핸들 저장
		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor);
		if (TargetASC && GameplayEffectClass)
		{
			FGameplayEffectContextHandle EffectContext = TargetASC->MakeEffectContext();
			EffectContext.AddSourceObject(this);

			FGameplayEffectSpecHandle SpecHandle = TargetASC->MakeOutgoingSpec(GameplayEffectClass, 1, EffectContext);
			if (SpecHandle.IsValid())
			{
				FActiveGameplayEffectHandle ActiveHandle = TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
				ActiveEffectHandles.Add(OtherActor, ActiveHandle);
			}
		}

		// 2) 지속형 Cue 실행
		InvokeGameplayCue(OtherActor);
	}
}

void AOverlapObject::OnTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!OtherActor || OtherActor == this)
		return;

	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor);
	if (!TargetASC)
		return;

	// 1) 지속형 GE 제거
	if (ActiveEffectHandles.Contains(OtherActor))
	{
		FActiveGameplayEffectHandle Handle = ActiveEffectHandles[OtherActor];
		TargetASC->RemoveActiveGameplayEffect(Handle);
		ActiveEffectHandles.Remove(OtherActor);
	}

	// 2) Cue 제거
	TargetASC->RemoveGameplayCue(GameplayCueTag);
}

void AOverlapObject::InvokeGameplayCue(AActor* Interactor)
{
	if (!Interactor) return;

	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Interactor);

	if (TargetASC)
	{
		FGameplayCueParameters Param;
		Param.SourceObject = this;
		Param.Instigator = Interactor;
		Param.Location = GetActorLocation();
		TargetASC->ExecuteGameplayCue(GameplayCueTag, Param);
	}
}
