
#include "SzObjects/ItemObject.h"
#include "Net/UnrealNetwork.h"
#include "Components/StaticMeshComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"

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

void AItemObject::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	ASC->InitAbilityActorInfo(this, this);
}

void AItemObject::OnInteractServer_Implementation(const APawn* Interactor)
{
	UseItemEffectServer(Interactor);
}

void AItemObject::OnInteractClient_Implementation(const APawn* Interactor)
{
	AInteractableObject::OnInteractClient_Implementation(Interactor);
	//UE_LOG(LogTemp, Log, TEXT("AItemObject::OnInteractClient"));
	UseItemEffectClient(Interactor);
}

void AItemObject::UseItemEffectServer_Implementation(const APawn* Interactor)
{
	// player가 아이템 먹었을 때 수행할 로직 구현
	InvokeGameplayCue(Interactor);		// 자기 자신 이펙트 재생
	ApplyEffectToTarget(Interactor);		// 상대방한테 게임플레이 이펙트 발동 시킴
}

void AItemObject::UseItemEffectClient_Implementation(const APawn* Interactor)
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

void AItemObject::ApplyEffectToTarget(const APawn* Interactor)
{	// 상대방에게 영향을 끼치는 기능
	
	AActor* TargetActor = const_cast<APawn*>(Interactor);

	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);

	if (TargetASC)
	{
		// 영향을 끼치기 위해서(게임플레이 이펙트를 보내기 위해서) 먼저 이펙트 컨텍스트를 제작해야함
		FGameplayEffectContextHandle EffectContext = TargetASC->MakeEffectContext();
		EffectContext.AddSourceObject(this); // 근데 오브젝트가 바로 사라져서 큰 의미 없음

		// 밖으로 내보낼 게임플레이 이펙트의 스펙을 만듦 => 이를 위해선 어떤 게임플레이 이펙트를 만들지 클래스를 지정해야 함
		FGameplayEffectSpecHandle EffectSpecHandle = TargetASC->MakeOutgoingSpec(GameplayEffectClass, 1, EffectContext);
		if (EffectSpecHandle.IsValid())
		{
			// BP_TargetASC에서 발동하는 것이기 때문에 ...Self 함수 사용
			TargetASC->BP_ApplyGameplayEffectSpecToSelf(EffectSpecHandle);
		}
	}
}

void AItemObject::InvokeGameplayCue(const APawn* Interactor)
{  // 자기 자신에게 특수효과를 재생

	AActor* TargetActor = const_cast<APawn*>(Interactor);

	FGameplayCueParameters Param;
	Param.SourceObject = this;
	Param.Instigator = TargetActor;
	Param.Location = GetActorLocation();
	ASC->ExecuteGameplayCue(GameplayCueTag, Param);	// 게임플레이 큐 용 태그를 설정할 땐, 헤더에서 메타 정보를 넣으면 좋음
}
