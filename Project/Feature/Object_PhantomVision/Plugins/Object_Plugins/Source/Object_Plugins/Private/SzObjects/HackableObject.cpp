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

	// Outline
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> MaterialFinder(
		TEXT("/Game/Project_TPT/Assets/Materials/M_Outline.M_Outline")
	);
	if (MaterialFinder.Succeeded())
	{
		OverlayMaterial = MaterialFinder.Object;
	}

	CurrentHackingPawn = nullptr;
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

	// Outline
	if (MeshComponent && OverlayMaterial)
	{
		// 동적 머티리얼 인스턴스 생성 및 OverlayMaterial로 적용
		OverlayMID = UMaterialInstanceDynamic::Create(OverlayMaterial, this);
		//MeshComponent->SetOverlayMaterial(OverlayMID);
		MeshComponent->OverlayMaterialMaxDrawDistance = MaxDrawDistance;

		// 파라미터 값 적용
		if (OverlayMID)
		{
			OverlayMID->SetVectorParameterValue(FName("OutlineColor"), OutlineColor);
			OverlayMID->SetScalarParameterValue(FName("LineScale"), LineScale);
		}
	}
}

void AHackableObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	if (!HackingComp) return;

	const float CurrentTime = GetWorld()->GetTimeSeconds();

	// 현재 해킹 중인 플레이어가 있을 때만 UpdateHackingProgress 호출
	if (CurrentHackingPawn && HackingComp->bIsHacking)
	{
		HackingComp->UpdateHackingProgress(CurrentHackingPawn, CurrentTime);
	}


	// 해킹된 상태에서 유지 시간이 지나면 초기화 (단, bKeepHacked가 false일 때만)
	if (HackingComp->bIsHacked && !HackingComp->bKeepHacked &&
		(CurrentTime - HackingComp->HackingStartTime >= HackingComp->HackedDuration))
	{
		HackingComp->CheckHackReset(CurrentHackingPawn);
		CurrentHackingPawn = nullptr; // 해킹이 리셋되면 현재 해킹 플레이어도 초기화
	}
}

void AHackableObject::OnHackingStartedServer_Implementation(APawn* Interactor)
{
	UE_LOG(LogTemp, Log, TEXT("AHackableObject::OnHackingStartedServer"));

	//if (!HackingComp || !Interactor) return;

	//// 현재 해킹 중인 플레이어 저장
	//CurrentHackingPawn = Interactor;

	//// HackingComponent에 Interactor 전달
	//HackingComp->HackingStarted(Interactor);
}

void AHackableObject::OnHackingStartedClient_Implementation(APawn* Interactor)
{
	UE_LOG(LogTemp, Log, TEXT("AHackableObject::OnHackingStartedClient"));

	if (!HackingComp || !Interactor) return;

	// 현재 해킹 중인 플레이어 저장
	CurrentHackingPawn = Interactor;

	// HackingComponent에 Interactor 전달
	HackingComp->HackingStarted(Interactor);
}


void AHackableObject::OnHackingCompletedServer_Implementation(APawn* Interactor)
{
	UE_LOG(LogTemp, Log, TEXT("AHackableObject::OnHackingCompletedServer"));

	//if (!HackingComp || !Interactor) return;

	//// 해킹을 시작한 플레이어와 완료하는 플레이어가 같은지 확인
	//if (CurrentHackingPawn != Interactor) return;

	//// HackingComponent에 Interactor 전달
	//HackingComp->HackingCompleted(Interactor);

	//// 해킹 완료 후 현재 해킹 플레이어 초기화
	//CurrentHackingPawn = nullptr;
}

void AHackableObject::OnHackingCompletedClient_Implementation(APawn* Interactor)
{
	UE_LOG(LogTemp, Log, TEXT("AHackableObject::OnHackingCompletedClient"));

	if (!HackingComp || !Interactor) return;

	// 해킹을 시작한 플레이어와 완료하는 플레이어가 같은지 확인
	if (CurrentHackingPawn != Interactor) return;

	// HackingComponent에 Interactor 전달
	HackingComp->HackingCompleted(Interactor);

	// 해킹 실패할때만
	if (HackingComp->bIsHacked == false)
	{
		CurrentHackingPawn = nullptr;
	}
}

bool AHackableObject::CanBeHacked_Implementation() const
{
	return !(HackingComp->bIsHacked) && !(HackingComp->bIsHacking);	// 해킹된 상태랑 해킹할 수 있는 상태는 반대.
}


void AHackableObject::ClearHacking_Implementation()
{
	// 해킹 초기화
	HackingComp->CheckHackReset();
	CurrentHackingPawn = nullptr;
}

void AHackableObject::SetWidgetVisibility_Implementation(bool Visible)
{
	if (WidgetComponent)
	{
		WidgetComponent->SetVisibility(Visible);
	}
}

void AHackableObject::SetOutline(bool bActive)
{
	if (bActive)
	{
		MeshComponent->SetOverlayMaterial(OverlayMID);
	}
	else
	{
		MeshComponent->SetOverlayMaterial(nullptr);
	}
}

