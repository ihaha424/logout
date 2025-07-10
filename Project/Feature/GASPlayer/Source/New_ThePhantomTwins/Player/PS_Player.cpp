// Fill out your copyright notice in the Description page of Project Settings.


#include "PS_Player.h"
#include "AbilitySystemComponent.h"
#include "New_ThePhantomTwins/Attribute/PlayerAttributeSet.h"

APS_Player::APS_Player()
{
	ASC = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("ASC"));

	ASC->SetIsReplicated(true);

	AttributeSet = CreateDefaultSubobject<UPlayerAttributeSet>(TEXT("AttributeSet"));
}

class UAbilitySystemComponent* APS_Player::GetAbilitySystemComponent() const
{
	return ASC;
}
