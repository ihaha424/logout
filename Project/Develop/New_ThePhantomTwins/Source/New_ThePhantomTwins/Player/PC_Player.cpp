// Fill out your copyright notice in the Description page of Project Settings.


#include "PC_Player.h"

#include "EnhancedInputSubsystems.h"

void APC_Player::BeginPlay()
{
	Super::BeginPlay();

	FInputModeGameOnly GameOnlyInputMode;
	SetInputMode(GameOnlyInputMode);

	if (IMC)
    {
        if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
        {
            if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
            {
                Subsystem->AddMappingContext(IMC, 0); // 우선순위는 필요에 따라 조정
            }
        }
    }
}
