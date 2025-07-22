
#include "SzObjects/ItemObject.h"
#include "Net/UnrealNetwork.h"
#include "AbilitySystemComponent.h"
#include "Components/StaticMeshComponent.h"

AItemObject::AItemObject() : AInteractableObject()
{
	bReplicates = true;

	ASC = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("ASC"));
	ASC->SetIsReplicated(true);
}

void AItemObject::BeginPlay()
{
	Super::BeginPlay();

}

class UAbilitySystemComponent* AItemObject::GetAbilitySystemComponent() const
{
	return ASC;
}

void AItemObject::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//DOREPLIFETIME(AItemObject, );
}

void AItemObject::OnInteractServer_Implementation(const APawn* Interactor)
{
	UseItemEffectServer();
}

void AItemObject::OnInteractClient_Implementation(const APawn* Interactor)
{
	AInteractableObject::OnInteractClient_Implementation(Interactor);
	//UE_LOG(LogTemp, Log, TEXT("AItemObject::OnInteractClient"));
	UseItemEffectClient();
}

void AItemObject::UseItemEffectServer_Implementation()
{
	//UE_LOG(LogTemp, Log, TEXT("AItemObject::UseItemEffectServer"));
}

void AItemObject::UseItemEffectClient_Implementation()
{
}

void AItemObject::DestroyItem_Implementation()
{
	if (UStaticMeshComponent* FoundMesh = FindComponentByClass<UStaticMeshComponent>())
	{
		FoundMesh->SetHiddenInGame(true);         // Mesh 숨김
		FoundMesh->SetVisibility(false);          // 완전히 안 보이도록 설정 (추가 안전 조치)
		FoundMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);  // 충돌 제거
	}

	SetActorEnableCollision(false);	// 더이상 이벤트가 일어나지 않도록 false
	SetLifeSpan(2.0f);				// 2초뒤에 자동으로 사라지도록.
}
