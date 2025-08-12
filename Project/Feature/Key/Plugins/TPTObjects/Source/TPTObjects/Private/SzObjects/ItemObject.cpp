
#include "SzObjects/ItemObject.h"
#include "Net/UnrealNetwork.h"
#include "Components/StaticMeshComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"

AItemObject::AItemObject() : AInteractableObject()
{
	bReplicates = true;
}

void AItemObject::BeginPlay()
{
	Super::BeginPlay();

}

void AItemObject::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//DOREPLIFETIME(AItemObject, );
}

void AItemObject::OnInteractServer_Implementation(const APawn* Interactor)
{
	InvokeGameplayCue(Interactor);			// 자기 자신 이펙트 재생
	ApplyEffectToTarget(Interactor);		// 상대방한테 게임플레이 이펙트 발동 시킴

	UseItemEffectServer(Interactor);

	DestroyItem();
}

void AItemObject::OnInteractClient_Implementation(const APawn* Interactor)
{
	AInteractableObject::OnInteractClient_Implementation(Interactor);

	UseItemEffectClient(Interactor);
}

void AItemObject::DestroyItem()
{
	bIsActived = true;

	if (UStaticMeshComponent* FoundMesh = FindComponentByClass<UStaticMeshComponent>())
	{
		FoundMesh->SetHiddenInGame(true);         // Mesh 숨김
		FoundMesh->SetVisibility(false);          // 완전히 안 보이도록 설정 (추가 안전 조치)
		FoundMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);  // 충돌 제거
	}

	if (UNiagaraComponent* NiagaraComp = FindComponentByClass<UNiagaraComponent>())
	{
		NiagaraComp->SetHiddenInGame(true);       // 파티클 렌더링 숨김
		NiagaraComp->Deactivate();                // 파티클 효과 비활성화
		NiagaraComp->SetVisibility(false);        // 안전 조치로 Visibility도 false
		NiagaraComp->SetComponentTickEnabled(false); // 로직 비활성화 (선택 사항)
	}

	SetActorEnableCollision(false);	// 더이상 이벤트가 일어나지 않도록 false
	SetLifeSpan(2.0f);				// 2초뒤에 자동으로 사라지도록.
}

void AItemObject::OnRep_bIsActived()
{
	if (UStaticMeshComponent* FoundMesh = FindComponentByClass<UStaticMeshComponent>())
	{
		FoundMesh->SetHiddenInGame(true);
		FoundMesh->SetVisibility(false);
		FoundMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void AItemObject::ApplyEffectToTarget(const APawn* Interactor)
{	// 상대방에게 영향을 끼치는 기능
	
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

void AItemObject::InvokeGameplayCue(const APawn* Interactor)
{  // 자기 자신에게 특수효과를 재생

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
