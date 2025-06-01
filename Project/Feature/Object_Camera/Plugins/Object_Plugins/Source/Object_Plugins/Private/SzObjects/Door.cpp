// Fill out your copyright notice in the Description page of Project Settings.


#include "SzObjects/Door.h"
#include "SzInterface/Hacking.h"

ADoor::ADoor() : ABaseObject()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ADoor::BeginPlay()
{
	Super::BeginPlay();

	// 필요한 활성화 수가 0이라면 트리거 개수 전체를 사용
	if (NeededActive == 0)
	{
		NeededActive = Triggers.Num();
	}
}

void ADoor::OnInteractSever_Implementation(APawn* Interactor)
{
	UE_LOG(LogTemp, Warning, TEXT("ADoor::OnInteract"));

	//if (CanInteract_Implementation(Interactor))
	//{
		UE_LOG(LogTemp, Warning, TEXT("ADoor::OnInteract3"));
		bIsOpened = true;

		// 이후 동작은 블루프린트에서 구현
		S2A_OpenDoor();
	//}
}

bool ADoor::CanInteract_Implementation(const APawn* Interactor) const
{
	UE_LOG(LogTemp, Warning, TEXT("ADoor::OnInteract2"));
	// 트리거가 모두 활성화되고, 열쇠가 있으면 상호작용 가능
	return AreAllObjActived() && HasKey(Interactor);
}

bool ADoor::AreAllObjActived() const
{
	// 활성화된 trigger 수를 세기 위한 변수
	int32 triggerActive = 0;

	// Door와 연결된 모든 trigger(Actors)를 하나씩 순회
	for (auto* trigger : Triggers)
	{
		// 유효한 액터인지 && IHacking 인터페이스를 구현했는지 확인
		if (trigger && trigger->GetClass()->ImplementsInterface(UHacking::StaticClass()))
		{
			// CanBeHacked()가 false면 = 더 이상 해킹할 수 없다 = 이미 해킹 완료된 상태로 간주
			if (IHacking::Execute_CanBeHacked(trigger) == false)
			{
				triggerActive++;
			}
		}
	}

	return triggerActive >= NeededActive;
}

bool ADoor::HasKey(const APawn* Interactor) const
{
	// 현재 key 확인이 불가능하니 테스트용으로 무조건 플레이어한테 열쇠가 있다고 가정.
	return true;
}

void ADoor::S2A_OpenDoor_Implementation()
{
	OpenDoor();
}
