// Fill out your copyright notice in the Description page of Project Settings.


#include "CM_LogOut.h"
#include "Player/PlayerCharacter.h"
#include "Attribute/PlayerAttributeSet.h"
#include "GameFramework/PlayerController.h"
#include "Player/PS_Player.h"
#include "GS_PhantomTwins.h"
#include "Engine/World.h"
#include "Log/TPTLog.h"
#include "Objects/InventoryComponent.h"

void UCM_LogOut::SetHP(float hp)
{
	APlayerCharacter* Character = Cast<APlayerCharacter>(GetPlayerController()->GetPawn());
	NULLCHECK_RETURN_LOG(Character, CMLog, Warning, );

	UAbilitySystemComponent* ASC = Character->GetAbilitySystemComponent();
	NULLCHECK_RETURN_LOG(ASC, CMLog, Warning, );

	if (const UPlayerAttributeSet* AttributeSet = Character->GetAbilitySystemComponent()->GetSet<UPlayerAttributeSet>())
	{
		ASC->SetNumericAttributeBase(UPlayerAttributeSet::GetHPAttribute(), hp);
	}
}

void UCM_LogOut::FullHP()
{
	APlayerCharacter* Character = Cast<APlayerCharacter>(GetPlayerController()->GetPawn());
	NULLCHECK_RETURN_LOG(Character, CMLog, Warning, );

	UAbilitySystemComponent* ASC = Character->GetAbilitySystemComponent();
	NULLCHECK_RETURN_LOG(ASC, CMLog, Warning, );

	if (const UPlayerAttributeSet* AttributeSet = Character->GetAbilitySystemComponent()->GetSet<UPlayerAttributeSet>())
	{
		ASC->SetNumericAttributeBase(UPlayerAttributeSet::GetHPAttribute(), AttributeSet->GetMaxHP());
	}
}

void UCM_LogOut::SetMental(float mental)
{
	APlayerCharacter* Character = Cast<APlayerCharacter>(GetPlayerController()->GetPawn());
	NULLCHECK_RETURN_LOG(Character, CMLog, Warning, );

	UAbilitySystemComponent* ASC = Character->GetAbilitySystemComponent();
	NULLCHECK_RETURN_LOG(ASC, CMLog, Warning, );

	if (const UPlayerAttributeSet* AttributeSet = Character->GetAbilitySystemComponent()->GetSet<UPlayerAttributeSet>())
	{
		ASC->SetNumericAttributeBase(UPlayerAttributeSet::GetMaxMentalPointAttribute(), mental);
	}
}

void UCM_LogOut::FullMental()
{
	APlayerCharacter* Character = Cast<APlayerCharacter>(GetPlayerController()->GetPawn());
	NULLCHECK_RETURN_LOG(Character, CMLog, Warning, );

	UAbilitySystemComponent* ASC = Character->GetAbilitySystemComponent();
	NULLCHECK_RETURN_LOG(ASC, CMLog, Warning, );

	if (const UPlayerAttributeSet* AttributeSet = Character->GetAbilitySystemComponent()->GetSet<UPlayerAttributeSet>())
	{
		ASC->SetNumericAttributeBase(UPlayerAttributeSet::GetMaxMentalPointAttribute(), AttributeSet->GetMaxMentalPoint());
	}
}

void UCM_LogOut::SetRecovery(bool bFlag)
{
	APlayerCharacter* Character = Cast<APlayerCharacter>(GetPlayerController()->GetPawn());
	NULLCHECK_RETURN_LOG(Character, CMLog, Warning, );

	UAbilitySystemComponent* ASC = Character->GetAbilitySystemComponent();
	NULLCHECK_RETURN_LOG(ASC, CMLog, Warning, );
	
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

void UCM_LogOut::GiveAllData()
{
	AGS_PhantomTwins* GS = GetWorld()->GetGameState<AGS_PhantomTwins>();
	NULLCHECK_RETURN_LOG(GS, CMLog, Warning, );

	GS->AddCollectedItem(GetPlayerController()->GetPawn(), 5);
}

void UCM_LogOut::MoveToExit()
{
	GetPlayerController()->GetPawn()->SetActorLocation(ExitPosition);
}

void UCM_LogOut::StartST1()
{

}

void UCM_LogOut::StartST2()
{

}

void UCM_LogOut::ClearNow()
{

}

void UCM_LogOut::GiveItems()
{
	APlayerCharacter* Character = Cast<APlayerCharacter>(GetPlayerController()->GetPawn());
	NULLCHECK_RETURN_LOG(Character, CMLog, Warning, );

	APS_Player* PS = Character->GetPlayerState<APS_Player>();
	NULLCHECK_RETURN_LOG(PS, CMLog, Warning, );

	for (int32 i = 0; i < 2; i++)
	{
		PS->InventoryComp->AddItem(EItemType::EMP);
		PS->InventoryComp->AddItem(EItemType::NoiseBomb);
	}
}

