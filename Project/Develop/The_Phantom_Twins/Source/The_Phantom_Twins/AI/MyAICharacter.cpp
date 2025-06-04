// Fill out your copyright notice in the Description page of Project Settings.


#include "MyAICharacter.h"

#include "AIInterface.h"
#include "MyAIController.h"
#include "Kismet/GameplayStatics.h"
#include "SplinePathActor.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense_Hearing.h"
#include "Perception/AISense_Sight.h"

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
	Tags.Add(FName("Object"));
}

void AMyAICharacter::OnHackingStarted_Implementation(APawn* Interactor)
{
	//UE_LOG(LogTemp, Warning, TEXT("A22222222222222222222222222I Is Hacked Task Executed"));
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
	//UE_LOG(LogTemp, Warning, TEXT("A222222222222222222222I Is Hacked Task Executed222222222222222222222"));
	BlackboardComp->SetValueAsEnum("AIState", static_cast<uint8>(EMyAIState::Hacked));
	// Perception 비활성화
	if (UAIPerceptionComponent* Perception = AIController->FindComponentByClass<UAIPerceptionComponent>())
	{
		Perception->SetSenseEnabled(UAISense_Sight::StaticClass(), false);
		Perception->SetSenseEnabled(UAISense_Hearing::StaticClass(), false);
		Perception->ForgetAll();
	}
	AIController->LastSightStartTime = 0.f;
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
