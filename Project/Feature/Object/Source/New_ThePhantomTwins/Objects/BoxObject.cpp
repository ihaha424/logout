#include "BoxObject.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GA/Object/GA_TrapBox.h"
#include "TimerManager.h"
#include "Components/ChildActorComponent.h"

ABoxObject::ABoxObject() : AInteractableObject()
{
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
	// Child Actor Component들을 검색해서 WarningClass와 일치하는 액터 찾기
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

	InvokeGameplayCue(Interactor);
	ApplyEffectToTarget(Interactor);

	if (bisTrapBox)
	{
		ExecuteTrapBoxGA(Interactor);

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
}

// 나머지 함수들은 동일...
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

void ABoxObject::ExecuteTrapBoxGA(const APawn* Interactor)
{
	if (!Interactor)
		return;

	AActor* TargetActor = const_cast<APawn*>(Interactor);

	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);

	if (!TargetASC) return;

	FGameplayEventData EventData;
	EventData.Instigator = Interactor;
	EventData.Target = Interactor;

	FGameplayAbilitySpec AbilitySpec(UGA_TrapBox::StaticClass(), 1);

	TargetASC->GiveAbilityAndActivateOnce(AbilitySpec, &EventData);
}
