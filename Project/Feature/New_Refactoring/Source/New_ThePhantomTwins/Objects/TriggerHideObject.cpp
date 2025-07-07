
#include "TriggerHideObject.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "SzComponents/OutlineComponent.h"
#include "Components/BoxComponent.h"

//AI Perception
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISense_Hearing.h"
#include "Perception/AISenseConfig_Hearing.h"

ATriggerHideObject::ATriggerHideObject()
{
    PrimaryActorTick.bCanEverTick = false;

    // Root Scene
    RootSceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
    SetRootComponent(RootSceneComp);

    // Mesh
    MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
    MeshComp->SetupAttachment(RootSceneComp);
    MeshComp->SetCollisionProfileName(TEXT("BlockAll"));

    // AIPerception과 player안의 sphere만 감지하는 Object
    SphereCollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
    SphereCollisionComp->SetupAttachment(RootSceneComp);
    SphereCollisionComp->SetSphereRadius(50.0f);
    SphereCollisionComp->SetCollisionObjectType(ECC_GameTraceChannel1); // Object Type 설정

    // Outline
    OutlineComp = CreateDefaultSubobject<UOutlineComponent>(TEXT("OutlineComponent"));

    // AI Perception
    StimuliSource = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("StimuliSource"));
    StimuliSource->bAutoRegister = true;
    StimuliSource->RegisterForSense(UAISense_Sight::StaticClass());
    StimuliSource->RegisterForSense(UAISense_Hearing::StaticClass());

    // "Interactable" 태그 추가
    Tags.Add(FName("Interactable"));

    BoxTriggerComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxTriggerComponent"));
    BoxTriggerComponent->SetCollisionProfileName(TEXT("OverlapAll"));
    BoxTriggerComponent->SetGenerateOverlapEvents(true);
    BoxTriggerComponent->SetupAttachment(RootSceneComp);
}

void ATriggerHideObject::BeginPlay()
{
    Super::BeginPlay();

    if (BoxTriggerComponent)
    {
        BoxTriggerComponent->OnComponentBeginOverlap.AddDynamic(this, &ATriggerHideObject::OnTriggerBeginOverlap);
        BoxTriggerComponent->OnComponentEndOverlap.AddDynamic(this, &ATriggerHideObject::OnTriggerEndOverlap);
    }
}

void ATriggerHideObject::OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    // 적과 아이템은 PlayerNum에 포함 안되도록 설정해야함
    HideStatus.PlayerNum = FMath::Clamp(HideStatus.PlayerNum + 1, 0, 2);
    HideStatus.bHasPlayer = (HideStatus.PlayerNum > 0);

    UE_LOG(LogTemp, Log, TEXT("TriggerHide :: Begin"));
}

void ATriggerHideObject::OnTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    HideStatus.PlayerNum = FMath::Clamp(HideStatus.PlayerNum - 1, 0, 2);
    HideStatus.bHasPlayer = (HideStatus.PlayerNum > 0);

    UE_LOG(LogTemp, Log, TEXT("TriggerHide :: End"));
}
