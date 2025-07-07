
#include "SzObjects/ItemObject.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AItemObject::AItemObject() : ABaseObject()
{
}


void AItemObject::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AItemObject, ItemStatus);
}

void AItemObject::BeginPlay()
{
	Super::BeginPlay();
}

void AItemObject::DestroyLogic_Implementation()
{

}
