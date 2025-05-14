// Fill out your copyright notice in the Description page of Project Settings.


#include "Object/TestObject.h"
#include "Components/WidgetComponent.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISense_Hearing.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Components/StaticMeshComponent.h"
#include "UObject\ConstructorHelpers.h"
#include "Engine\StaticMesh.h"

// Sets default values
ATestObject::ATestObject()
{
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ObjectMesh"));
	Mesh->SetCollisionProfileName(TEXT("TestObjectMesh"));
	RootComponent = Mesh;

	// mesh reference
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ObjectMeshRef(TEXT("/PlayerPlugin/LevelPrototyping/Meshes/SM_ChamferCube.SM_ChamferCube"));
	if (ObjectMeshRef.Object)
	{
		Mesh->SetStaticMesh(ObjectMeshRef.Object.Get());
	}

    Widget = CreateDefaultSubobject<UWidgetComponent>(TEXT("ObjectWidget"));
    Widget->SetupAttachment(RootComponent);
    Widget->SetWidgetSpace(EWidgetSpace::Screen);
    Widget->SetDrawSize(FVector2D(10, 10));
    Widget->SetRelativeLocation(FVector(0, 0, 100));

    static ConstructorHelpers::FClassFinder<UUserWidget> WidgetRef(TEXT("/PlayerPlugin/MyProject/Blueprints/UI/NewWidgetBlueprint.NewWidgetBlueprint_C"));
    if (WidgetRef.Class)
    {
        Widget->SetWidgetClass(WidgetRef.Class);
    }

    Widget->SetVisibility(false);

	//// StimuliSource는 AI 감지 시스템에 등록하기 위한 컴포넌트
	//StimuliSource = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("StimuliSource"));

	//StimuliSource->bAutoRegister = true;

	//// StimuliSource에 감지할 감각을 등록
	//StimuliSource->RegisterForSense(UAISense_Sight::StaticClass());
	//StimuliSource->RegisterForSense(UAISense_Hearing::StaticClass());

	////StimuliSource->RegisterWithPerceptionSystem();
}

