// Fill out your copyright notice in the Description page of Project Settings.


#include "PS_Player.h"
#include "AbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"
#include "New_ThePhantomTwins/Attribute/PlayerAttributeSet.h"
#include "../Objects/InventoryComponent.h"
#include "SaveGame/TPTSaveGameHelperLibrary.h"
#include "Tags/TPTGameplayTags.h"
#include "Player/PlayerCharacter.h"

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
	DOREPLIFETIME(APS_Player, TeamID);
}

//void APS_Player::CopyProperties(APlayerState* PlayerState)
//{
//	Super::CopyProperties(PlayerState);
//	APS_Player* NewPS = Cast<APS_Player>(PlayerState);
//	if (NewPS)
//	{
//		NewPS->IdentifyCharacterData = IdentifyCharacterData;
//		NewPS->InventoryComp = InventoryComp;
//		NewPS->TeamID = TeamID;
//		NewPS->ASC = ASC;
//		NewPS->AttributeSet = AttributeSet;
//	}
//}

void APS_Player::SetIdentifyCharacterData()
{
	UTPTSaveGame* TPTLocalPlayerSaveGame = UTPTSaveGameHelperLibrary::GetSaveGameData<UTPTSaveGame>();
	IdentifyCharacterData = TPTLocalPlayerSaveGame->IdentifyCharacterData;
}

class UAbilitySystemComponent* APS_Player::GetAbilitySystemComponent() const
{
	return ASC;
}
