// Fill out your copyright notice in the Description page of Project Settings.


#include "PS_Player.h"
#include "AbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"
#include "New_ThePhantomTwins/Attribute/PlayerAttributeSet.h"
#include "../Objects/InventoryComponent.h"
#include "Player/PlayerCharacter.h"
#include "Log/TPTLog.h"
#include "Blueprint/UserWidget.h"
#include "Components/WidgetComponent.h"

APS_Player::APS_Player()
{
	ASC = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("ASC"));

	ASC->SetIsReplicated(true);

	AttributeSet = CreateDefaultSubobject<UPlayerAttributeSet>(TEXT("AttributeSet"));

	TeamID = FGenericTeamId(1);

	InventoryComp = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory"));
}

void APS_Player::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APS_Player, ASC);
	DOREPLIFETIME(APS_Player, AttributeSet);
	DOREPLIFETIME(APS_Player, PassiveSkillTag);
	DOREPLIFETIME(APS_Player, ActiveSkillTag);
	DOREPLIFETIME(APS_Player, TeamID);
	DOREPLIFETIME(APS_Player, bIsRecovery);
	DOREPLIFETIME(APS_Player, bIsGroggy);
}

void APS_Player::SetRecovery(bool IsRecovery)
{
	bIsRecovery = IsRecovery;
}

void APS_Player::SetGroggy(bool IsGroggy)
{
	//APlayerCharacter* Character = Cast<APlayerCharacter>(GetOwner());
	//NULLCHECK_RETURN_LOG(Character, PlayerLog, Error, )

	//	if (IsGroggy)
	//	{
	//		Character->DownedWidget->GetUserWidgetObject()->SetVisibility(ESlateVisibility::Visible);
	//	}
	//	else
	//	{
	//		Character->DownedWidget->GetUserWidgetObject()->SetVisibility(ESlateVisibility::Hidden);
	//	}

	bIsRecovery = IsGroggy;
}

class UAbilitySystemComponent* APS_Player::GetAbilitySystemComponent() const
{
	return ASC;
}
