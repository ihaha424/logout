// Fill out your copyright notice in the Description page of Project Settings.


#include "GA/Action/GA_Interact.h"
#include "Player/PlayerCharacter.h"
#include "Player/PS_Player.h"
#include "Attribute/PlayerAttributeSet.h"
#include "Tags/TPTGameplayTags.h"
#include "Log/TPTLog.h"

void UGA_Interact::InteractiveObjectCheck()
{
//	FVector Start = GetActorLocation();
//	FVector End = Sphere->GetComponentLocation();
//
//	FHitResult Hit;
//	FCollisionQueryParams Params;
//	Params.AddIgnoredActor(this); // 자기 자신은 무시
//	Params.AddIgnoredComponent(this->RecoverySphere); // 자기 자신은 무시
//
//	FCollisionObjectQueryParams ObjParams;
//	ObjParams.AddObjectTypesToQuery(ECC_WorldStatic);
//	ObjParams.AddObjectTypesToQuery(ECC_GameTraceChannel1);
//
//	bool bHit = GetWorld()->LineTraceSingleByObjectType(
//		Hit,
//		Start,
//		End,
//		ObjParams,
//		Params
//	);
//
//#if WITH_EDITOR
//	//DrawDebugLine(GetWorld(), Start, End, Hit.GetActor() == TargetActor ? FColor::Blue : FColor::Silver, false, 1.0f, 0, 0.3f);
//#endif
//
//	// Ray가 정확히 TargetActor에 부딪혔는지 확인
//	bHit && Hit.GetActor() == TargetActor;
}
