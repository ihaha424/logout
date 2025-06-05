// Fill out your copyright notice in the Description page of Project Settings.


#include "MyAICharacter.h"

#include "AIInterface.h"
#include "MyAIStateWidget.h"
#include "MyAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense_Hearing.h"
#include "Perception/AISense_Sight.h"
#include "The_Phantom_Twins/Player/PlayerBase.h"

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
	AIStateWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("AIStateWidget"));
	AIStateWidget->SetupAttachment(GetMesh(), FName("head"));
	AIStateWidget->SetRelativeLocation(FVector(0.f, 0.f, 200));
	AIStateWidget->SetWidgetSpace(EWidgetSpace::World);
	AIStateWidget->SetDrawAtDesiredSize(true);

	static ConstructorHelpers::FClassFinder<UUserWidget> WidgetClass(TEXT("/Game/Project_TPT/Assets/UI/WB_AIState"));
	if (WidgetClass.Succeeded())
	{
		AIStateWidget->SetWidgetClass(WidgetClass.Class);
	}

	Tags.Add(FName("Object"));
}

void AMyAICharacter::OnHackingStarted_Implementation(APawn* Interactor)
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
	APlayerBase* Target = Cast<APlayerBase>(BlackboardComp->GetValueAsObject(TEXT("TargetPlayer")));
	if (Target == nullptr || Target != Interactor)
	{
		// AI 상태를 해킹 상태로 변경
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
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("AI Hacking Failed: Interactor is the Target Player"));
	}
	
}

void AMyAICharacter::UpdateAIStateWidget(EAIStateWidget State)
{
	if (!AIStateWidget) return;

	UUserWidget* Widget = AIStateWidget->GetUserWidgetObject();
	if (UMyAIStateWidget* Nameplate = Cast<UMyAIStateWidget>(Widget))
	{
		Nameplate->SetState(State);
	}
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
