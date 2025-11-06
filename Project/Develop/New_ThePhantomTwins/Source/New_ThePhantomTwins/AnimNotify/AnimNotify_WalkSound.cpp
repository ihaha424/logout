// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify/AnimNotify_WalkSound.h"
#include "GS_PhantomTwins.h"
#include "Kismet/GameplayStatics.h"
#include "Log/TPTLog.h"

FString UAnimNotify_WalkSound::GetNotifyName_Implementation() const
{
	return TEXT("WalkSound");
}

void UAnimNotify_WalkSound::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	if (!MeshComp || StepSoundByMap.Num() == 0) return;

	AActor* Actor = MeshComp->GetOwner();
	if (!Actor)
		return;

	UWorld* World = Actor->GetWorld();
	if (!World)
		return;

	AGS_PhantomTwins* GS = World->GetGameState<AGS_PhantomTwins>();
	if (!GS)
		return;

	EMapType mapData = GS->GetMapData();
	const float Loundness = 1.f;

	if (StepSoundByMap.Contains(mapData))
	{
		UGameplayStatics::PlaySoundAtLocation(Actor, StepSoundByMap[mapData].stepSound, Actor->GetActorLocation());
		Actor->MakeNoise(Loundness, nullptr, Actor->GetActorLocation(), 0.f, StepSoundByMap[mapData].Type);
	}
	else
	{
		UGameplayStatics::PlaySoundAtLocation(Actor, StepSoundByMap[EMapType::ST1].stepSound, Actor->GetActorLocation());
		Actor->MakeNoise(Loundness, nullptr, Actor->GetActorLocation(), 0.f);
	}
}
