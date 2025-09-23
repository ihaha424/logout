
#include "TriggerHideObject.h"
#include "Net/UnrealNetwork.h"
#include "Components/BoxComponent.h"
#include "../Log/TPTLog.h"

//AI Perception
#include "Components/SphereComponent.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISense_Hearing.h"
#include "Perception/AISenseConfig_Hearing.h"

ATriggerHideObject::ATriggerHideObject() : ABaseObject()
{
    bReplicates = true;

    BoxTriggerComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxTriggerComponent"));
    BoxTriggerComp->SetCollisionProfileName(TEXT("OverlapAll"));
    BoxTriggerComp->SetGenerateOverlapEvents(true);
    BoxTriggerComp->SetupAttachment(RootSceneComp);

    // AIPerception과 player안의 sphere만 감지하는 Object
    SphereCollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
    SphereCollisionComp->SetupAttachment(RootSceneComp);
    SphereCollisionComp->SetSphereRadius(50.0f);
    SphereCollisionComp->SetCollisionObjectType(ECC_GameTraceChannel1); // Object Type 설정

    // AI Perception
    StimuliSource = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("StimuliSource"));
    StimuliSource->bAutoRegister = true;
    StimuliSource->RegisterForSense(UAISense_Sight::StaticClass());
    StimuliSource->RegisterForSense(UAISense_Hearing::StaticClass());

}

void ATriggerHideObject::BeginPlay()
{
    Super::BeginPlay();

    if (BoxTriggerComp)
    {
        BoxTriggerComp->OnComponentBeginOverlap.AddDynamic(this, &ATriggerHideObject::OnTriggerBeginOverlap);
        BoxTriggerComp->OnComponentEndOverlap.AddDynamic(this, &ATriggerHideObject::OnTriggerEndOverlap);
    }
}

void ATriggerHideObject::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ATriggerHideObject, bHasPlayer);
    DOREPLIFETIME(ATriggerHideObject, HidePlayerNum);
}

void ATriggerHideObject::OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    // 적과 아이템은 PlayerNum에 포함 안되도록 설정해야함
    HidePlayerNum = FMath::Clamp(HidePlayerNum + 1, 0, 2);
    bHasPlayer = (HidePlayerNum > 0);

    //TPT_LOG(ObjectLog, Log, TEXT("TriggerHide :: Begin"));
}

void ATriggerHideObject::OnTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    HidePlayerNum = FMath::Clamp(HidePlayerNum - 1, 0, 2);
	bHasPlayer = (HidePlayerNum > 0);

    //TPT_LOG(ObjectLog, Log, TEXT("TriggerHide :: End"));
}
