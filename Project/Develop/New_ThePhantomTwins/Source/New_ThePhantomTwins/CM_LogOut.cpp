// Fill out your copyright notice in the Description page of Project Settings.


#include "CM_LogOut.h"
#include "Player/PlayerCharacter.h"
#include "Attribute/PlayerAttributeSet.h"
#include "GameFramework/PlayerController.h"


void UCM_LogOut::SetHP(float hp)
{
	if (APlayerCharacter* Character = Cast<APlayerCharacter>(GetPlayerController()->GetPawn()))
	{
		if (UAbilitySystemComponent* ASC = Character->GetAbilitySystemComponent())
		{
			if (const UPlayerAttributeSet* AttributeSet = Character->GetAbilitySystemComponent()->GetSet<UPlayerAttributeSet>())
			{
				ASC->SetNumericAttributeBase(UPlayerAttributeSet::GetHPAttribute(), hp);
			}
		}
	}
}

void UCM_LogOut::SetMaxHP()
{
	if (APlayerCharacter* Character = Cast<APlayerCharacter>(GetPlayerController()->GetPawn()))
	{
		if (UAbilitySystemComponent* ASC = Character->GetAbilitySystemComponent())
		{
			if (const UPlayerAttributeSet* AttributeSet = Character->GetAbilitySystemComponent()->GetSet<UPlayerAttributeSet>())
			{
				ASC->SetNumericAttributeBase(UPlayerAttributeSet::GetHPAttribute(), AttributeSet->GetMaxHP());
			}
		}
	}
}

void UCM_LogOut::SetRecovery(bool bFlag)
{
	if (APlayerCharacter* Character = Cast<APlayerCharacter>(GetPlayerController()->GetPawn()))
	{
		if (UAbilitySystemComponent* ASC = Character->GetAbilitySystemComponent())
		{
			if (bFlag)
			{
				if (Character->GetClass()->ImplementsInterface(UInteract::StaticClass()))
				{
					if (Character->HasAuthority())
					{
						IInteract::Execute_OnInteractServer(Character, Character);
					}
					IInteract::Execute_OnInteractClient(Character, Character);
				}
			}
			else
			{
				SetHP(0);
			}
		}
	}
}
