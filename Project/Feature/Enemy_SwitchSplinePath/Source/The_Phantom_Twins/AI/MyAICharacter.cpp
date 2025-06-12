// Fill out your copyright notice in the Description page of Project Settings.


#include "MyAICharacter.h"

#include <Kismet/GameplayStatics.h>

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

	bReplicates = true;
	bAlwaysRelevant = true;
	PrimaryActorTick.bCanEverTick = true;
	bUseControllerRotationYaw = true;
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->bOrientRotationToMovement = false;
	}
	// 위젯컴포넌트에 대한 설정
	AIStateWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("AIStateWidget"));
	AIStateWidget->SetupAttachment(RootComponent);
	AIStateWidget->SetRelativeLocation(FVector(0.f, 0.f, 200));
	AIStateWidget->SetWidgetSpace(EWidgetSpace::World);
	AIStateWidget->SetDrawAtDesiredSize(true);
	static ConstructorHelpers::FClassFinder<UUserWidget> WidgetClass(TEXT("/Game/Project_TPT/Assets/UI/WB_AIWidget"));
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
	APlayerBase* Target = Cast<APlayerBase>(BlackboardComp->GetValueAsObject(TEXT("ChasingPlayer")));
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
		AIController->ResetStimulus();
	}
	
}

void AMyAICharacter::S2A_UpdateAIStateWidget_Implementation(EAIStateWidget State)
{
	if (!AIStateWidget) return;

	UUserWidget* Widget = AIStateWidget->GetUserWidgetObject();
	if (UMyAIStateWidget* Nameplate = Cast<UMyAIStateWidget>(Widget))
	{
		Nameplate->SetState(State);
	}
}

void AMyAICharacter::S2A_UpdateWidgetDirection_Implementation(FRotator Rotate)
{
	if (!AIStateWidget) return;

	AIStateWidget->SetWorldRotation(Rotate);
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
	if (GetNetMode() != NM_DedicatedServer) //서버에서는 위젯 회전 렌더링 X
	{
		// 렌더링 타임에 AIStateWidget의 회전 업데이트
		if (AIStateWidget)
		{
			APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
			if (CameraManager)
			{
				FVector CameraLocation = CameraManager->GetCameraLocation();
				FVector WidgetLocation = AIStateWidget->GetComponentLocation();
				FRotator LookAtRotation = (CameraLocation - WidgetLocation).Rotation();
				S2A_UpdateWidgetDirection(LookAtRotation);
			}
		}
	}
  
}

// Called to bind functionality to input
void AMyAICharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}
