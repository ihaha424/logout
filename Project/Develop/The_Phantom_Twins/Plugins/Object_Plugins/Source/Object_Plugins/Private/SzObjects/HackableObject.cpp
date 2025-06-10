// Fill out your copyright notice in the Description page of Project Settings.


#include "SzObjects/HackableObject.h"
#include "Blueprint/UserWidget.h"
#include "Components/WidgetComponent.h"
#include "SzComponents/HackableComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "SzUI/HackingGauge.h"

// Sets default values
AHackableObject::AHackableObject()
{
	PrimaryActorTick.bCanEverTick = true;

	// "Object" 태그 추가
	Tags.Add(FName("Object"));

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	SetRootComponent(MeshComponent);

	// WidgetComponent
	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("ObjectWidget"));
	WidgetComponent->SetupAttachment(RootComponent);
	WidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	WidgetComponent->SetDrawSize(FVector2D(10, 10));
	WidgetComponent->SetRelativeLocation(FVector(0, 0, 100));
	WidgetComponent->SetVisibility(false); // 기본은 비활성화


	// AIPerception과 player안의 sphere만 감지하는 Object
	SphereCollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	SphereCollisionComp->SetupAttachment(RootComponent);
	SphereCollisionComp->ComponentTags.Add(FName("Object"));
	SphereCollisionComp->SetSphereRadius(50.0f);
	SphereCollisionComp->SetCollisionObjectType(ECC_GameTraceChannel1); // Object Type 설정
}

// Called when the game starts or when spawned
void AHackableObject::BeginPlay()
{
	Super::BeginPlay();

	if (!HackingComp)
	{
		HackingComp = FindComponentByClass<UHackableComponent>();
		if (!HackingComp)
		{
			UE_LOG(LogTemp, Warning, TEXT("No HackingComp found on %s"), *GetName());
		}
	}

	if (WidgetClass)
	{
		WidgetComponent->SetWidgetClass(WidgetClass);
		WidgetComponent->SetVisibility(true);
	}
}

void AHackableObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const float CurrentTime = GetWorld()->GetTimeSeconds();

	HackingComp->UpdateHackingProgress(CurrentTime);

	// 해킹된 상태에서 유지 시간이 지나면 초기화 (단, bKeepHacked가 false일 때만)
	if (HackingComp->bIsHacked && !HackingComp->bKeepHacked &&
		(CurrentTime - HackingComp->HackingStartTime >= HackingComp->HackedDuration))
	{
		HackingComp->CheckHackReset();
	}
}

void AHackableObject::OnHackingStarted_Implementation(APawn* Interactor)
{
	HackingComp->HackingStarted();
}


void AHackableObject::OnHackingCompleted_Implementation(APawn* Interactor)
{
	HackingComp->HackingCompleted();
}

bool AHackableObject::CanBeHacked_Implementation() const
{
	return !(HackingComp->bIsHacked);	// 해킹된 상태랑 해킹할 수 있는 상태는 반대.
}


void AHackableObject::ClearHacking_Implementation()
{
	// 해킹 초기화
	HackingComp->CheckHackReset();
}

void AHackableObject::SetWidgetVisibility_Implementation(bool Visible)
{
	WidgetComponent->SetVisibility(Visible);
}