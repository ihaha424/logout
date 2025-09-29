#include "BoxObject.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GA/Object/GA_TrapBox.h"
#include "TimerManager.h"
#include "Components/ChildActorComponent.h"
#include "Net/UnrealNetwork.h"

ABoxObject::ABoxObject() : AInteractableObject()
{
	// 복제 설정 확인
	bReplicates = true;
}

void ABoxObject::BeginPlay()
{
	Super::BeginPlay();

	// Child Actor Component의 Warning Actor를 찾아서 숨김
	AActor* WarningActor = FindWarningActor();
	if (WarningActor)
	{
		WarningActor->SetActorHiddenInGame(true);
		WarningActor->SetActorEnableCollision(false);
	}
}

AActor* ABoxObject::FindWarningActor()
{
	TArray<UChildActorComponent*> ChildActorComponents;
	GetComponents<UChildActorComponent>(ChildActorComponents);

	for (UChildActorComponent* ChildActorComp : ChildActorComponents)
	{
		if (ChildActorComp && ChildActorComp->GetChildActor())
		{
			AActor* ChildActor = ChildActorComp->GetChildActor();
			if (WarningClass && ChildActor->GetClass() == WarningClass)
			{
				return ChildActor;
			}
		}
	}
	return nullptr;
}

void ABoxObject::OnInteractServer_Implementation(const APawn* Interactor)
{
	if (bIsActived) return;

	// 서버에서 멀티캐스트 함수들 호출
	MulticastInvokeGameplayCue(Interactor);
	MulticastApplyEffect(Interactor);

	if (bisTrapBox)
	{
		MulticastExecuteTrapBox(Interactor);
		MulticastShowWarning();
	}
}

// 멀티캐스트 함수들 구현
void ABoxObject::MulticastApplyEffect_Implementation(const APawn* Interactor)
{
	ApplyEffectToTarget(Interactor);
}

void ABoxObject::MulticastInvokeGameplayCue_Implementation(const APawn* Interactor)
{
	InvokeGameplayCue(Interactor);
}

void ABoxObject::MulticastExecuteTrapBox_Implementation(const APawn* Interactor)
{
	ExecuteTrapBoxGA(Interactor);
}

void ABoxObject::MulticastShowWarning_Implementation()
{
	if (!bisTrapBox) return;

	AActor* WarningActor = FindWarningActor();
	if (WarningActor && !WarningActor->IsActorBeingDestroyed())
	{
		WarningActor->SetActorHiddenInGame(false);
		WarningActor->SetActorEnableCollision(true);

		FTimerHandle TimerHandle;
		FTimerDelegate TimerDel;
		TimerDel.BindLambda([WarningActor]()
			{
				if (WarningActor && !WarningActor->IsActorBeingDestroyed())
				{
					WarningActor->SetActorHiddenInGame(true);
					WarningActor->SetActorEnableCollision(false);
				}
			});
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDel, 5.f, false);
	}
}

// 기존 함수들은 그대로 유지
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
	if (!Interactor) return;

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

void ABoxObject::ExecuteTrapBoxGA(const APawn* Interactor)
{
	if (!Interactor) return;

	AActor* TargetActor = const_cast<APawn*>(Interactor);
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);

	if (!TargetASC) return;

	FGameplayEventData EventData;
	EventData.Instigator = Interactor;
	EventData.Target = Interactor;

	FGameplayAbilitySpec AbilitySpec(UGA_TrapBox::StaticClass(), 1);
	TargetASC->GiveAbilityAndActivateOnce(AbilitySpec, &EventData);
}
