// Fill out your copyright notice in the Description page of Project Settings.


#include "SzComponents/HideComponent.h"
#include "SzObjects/BaseObject.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Pawn.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h" 

// Sets default values for this component's properties
UHideComponent::UHideComponent()
{
	UE_LOG(LogTemp, Log, TEXT("UHideComponent::UHideComponent()"));

	PrimaryComponentTick.bCanEverTick = false;

	TriggerComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxTriggerComponent"));
	TriggerComponent->SetupAttachment(this);
	TriggerComponent->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	TriggerComponent->SetGenerateOverlapEvents(true);
}


// Called when the game starts
void UHideComponent::BeginPlay()
{
	UE_LOG(LogTemp, Log, TEXT("UHideComponent::BeginPlay()"));

	Super::BeginPlay();

	if (!TriggerComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("TriggerComponent is null in HideComponent"));
		return;
	}

	AActor* Owner = GetOwner();

	if (Owner)
	{
		UStaticMeshComponent* MeshComp = Owner->FindComponentByClass<UStaticMeshComponent>();
		if (MeshComp && MeshComp->GetStaticMesh()) {
			FVector Extent = MeshComp->GetStaticMesh()->GetBounds().BoxExtent;
			TriggerComponent->SetBoxExtent(Extent);
		}
	}

	TriggerComponent->OnComponentBeginOverlap.AddDynamic(this, &UHideComponent::OnTriggerBeginOverlap);
	TriggerComponent->OnComponentEndOverlap.AddDynamic(this, &UHideComponent::OnTriggerEndOverlap);
}

void UHideComponent::Execute(APawn* Interactor)
{
	UE_LOG(LogTemp, Log, TEXT("UHideComponent::Execute"));


}

void UHideComponent::OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Log, TEXT("Entered TriggerComponent: %s"), *OtherActor->GetName());

	APawn* Pawn = Cast<APawn>(OtherActor);
	if (Pawn)
	{
		bHasPlayer = true;
		UE_LOG(LogTemp, Log, TEXT("Player %s entered hiding spot."), *Pawn->GetName());
	}
}

void UHideComponent::OnTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UE_LOG(LogTemp, Log, TEXT("Exited TriggerComponent: %s"), *OtherActor->GetName());

	APawn* Pawn = Cast<APawn>(OtherActor);
	if (Pawn)
	{
		bHasPlayer = false;
		UE_LOG(LogTemp, Log, TEXT("Player %s exited hiding spot."), *Pawn->GetName());
	}
}
