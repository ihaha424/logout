// Fill out your copyright notice in the Description page of Project Settings.


#include "TextObject.h"

ATextObject::ATextObject() : AInteractableObject()
{

}

void ATextObject::BeginPlay()
{

}

void ATextObject::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void ATextObject::OnRep_bIsActived()
{
    // 모든 플레이어에게 "오래된 데이터 획득" UI 출력
}

void ATextObject::SetData2Widget_Implementation()
{

}
