// Fill out your copyright notice in the Description page of Project Settings.


#include "TestItemBox.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
ATestItemBox::ATestItemBox()
{
	ASC = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("ASC"));
	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));

	RootComponent = Trigger;
	Mesh->SetupAttachment(Trigger);

	Trigger->SetCollisionProfileName(TEXT("ABTrigger"));
	Trigger->SetBoxExtent(FVector(40.0f, 42.0f, 30.0f));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> BoxMeshRef(TEXT("/Script/Engine.StaticMesh'/Game/StarterContent/Shapes/Shape_Cube.Shape_Cube'"));
	if (BoxMeshRef.Object)
	{
		Mesh->SetStaticMesh(BoxMeshRef.Object);
	}
	Mesh->SetRelativeLocation(FVector(0.0f, -3.5f, -30.0f));
	Mesh->SetCollisionProfileName(TEXT("NoCollision"));

	bReplicates = true;
	bNetLoadOnClient = true;
}

void ATestItemBox::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	InvokeGameplayCue(OtherActor);
	ApplyEffectToTarget(OtherActor);
	// 먹고나서 더이상 상호작용 할수없도록 하는것
	Mesh->SetHiddenInGame(true);
	SetActorEnableCollision(false);
	SetLifeSpan(2.0f);
}

void ATestItemBox::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	// 가지고 있는 어빌리티 정보 초기화.
	ASC->InitAbilityActorInfo(this, this);

}

UAbilitySystemComponent* ATestItemBox::GetAbilitySystemComponent() const
{
	return ASC;
}


void ATestItemBox::ApplyEffectToTarget(AActor* Target)
{
	// 타겟의 ASC 가져오기
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Target);
	if (TargetASC)
	{	//GA없이 이펙트를 발동시키는 그거다.
		FGameplayEffectContextHandle EffectContext = TargetASC->MakeEffectContext();
		EffectContext.AddSourceObject(this);
		// 밖으로 내보낼 스펙 만드는중.
		FGameplayEffectSpecHandle EffectSpecHandle = TargetASC->MakeOutgoingSpec(GameplayEffectClass, 1, EffectContext);
		if (EffectSpecHandle.IsValid())
		{
			TargetASC->BP_ApplyGameplayEffectSpecToSelf(EffectSpecHandle);

			// TargetASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get()); 위랑 아래랑 똑같다.
		}
	}
}

void ATestItemBox::InvokeGameplayCue(AActor* Target)
{
	FGameplayCueParameters Param;
	Param.SourceObject = this;
	Param.Instigator = Target;
	Param.Location = GetActorLocation();
	ASC->ExecuteGameplayCue(GameplayCueTag, Param);
}

