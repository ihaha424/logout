// Fill out your copyright notice in the Description page of Project Settings.


#include "MyAICharacter.h"

#include "AIInterface.h"
#include "MyAIController.h"
#include "Kismet/GameplayStatics.h"
#include "SplinePathActor.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
AMyAICharacter::AMyAICharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bUseControllerRotationYaw = true;
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->bOrientRotationToMovement = false;
	}
}

void AMyAICharacter::OnHackingStarted_Implementation()
{
	AMyAIController* AIController = Cast<AMyAIController>(GetController());
	if (!AIController)
	{
		return;
	}
	UBlackboardComponent* BlackboardComp = AIController->GetBlackboardComponent();
	if (!BlackboardComp)
	{
		return;
	}
	BlackboardComp->SetValueAsEnum("AIState", static_cast<uint8>(EMyAIState::Hacked));

}

// Called when the game starts or when spawned
void AMyAICharacter::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AMyAICharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
  
  
}

// Called to bind functionality to input
void AMyAICharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}
