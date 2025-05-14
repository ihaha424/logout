// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/CharacterBase.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/WidgetComponent.h"
#include "UI/PlayerWidgetComponent.h"
#include "CharacterStat/PlayerStatComponent.h"
#include "UI/PlayerHpBar.h"
#include "UObject\ConstructorHelpers.h"
#include "Animation/AnimationAsset.h"
#include "Animation/AnimInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine\SkeletalMesh.h"

// Sets default values
ACharacterBase::ACharacterBase()
{
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.f);
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("Pawn"));

	// movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0, 500, 0);
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 200.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 0.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// mesh
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -50.f), FRotator(0.f, -90.f, 0.f));
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	GetMesh()->SetCollisionProfileName(TEXT("CharacterMesh"));

	// mesh reference
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> CharacterMeshRef(TEXT("/PlayerPlugin/Characters/Mannequins/Meshes/SKM_Quinn_Simple.SKM_Quinn_Simple"));
	if (CharacterMeshRef.Object)
	{
		GetMesh()->SetSkeletalMesh(CharacterMeshRef.Object);
	}

	// anim instance reference
	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimInstanceClassRef(TEXT("/PlayerPlugin/Characters/Mannequins/Animations/ABP_Quinn.ABP_Quinn_C"));
	if (AnimInstanceClassRef.Class)
	{
		GetMesh()->SetAnimInstanceClass(AnimInstanceClassRef.Class);
	}

	/////////////////////////////////////////////////////////////////

	// Stat
	Stat = CreateDefaultSubobject<UPlayerStatComponent>(TEXT("Stat"));

	// UI Widget
	GroggyWidget = CreateDefaultSubobject<UPlayerWidgetComponent>(TEXT("Widget"));
	GroggyWidget->SetupAttachment(GetMesh());


	static ConstructorHelpers::FClassFinder<UUserWidget> GroggyWidgetRef(TEXT("/PlayerPlugin/MyProject/Blueprints/UI/WB_GroggyWidget.WB_GroggyWidget_C"));
	if (GroggyWidgetRef.Class)
	{
		GroggyWidget->SetWidgetClass(GroggyWidgetRef.Class);
		GroggyWidget->SetWidgetSpace(EWidgetSpace::Screen);
		GroggyWidget->SetDrawSize(FVector2D(150.f, 80.f));
		GroggyWidget->SetRelativeLocation(FVector(0, 0, 0));
		GroggyWidget->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GroggyWidget->SetVisibility(false);
		UE_LOG(LogTemp, Warning, TEXT("Character is Groggy Initialize"));
	}
}

void ACharacterBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	Stat->OnHpZero.AddUObject(this, &ACharacterBase::SetGroggy);
}

void ACharacterBase::SetGroggy()
{
	GroggyWidget->SetVisibility(true);
	GetCharacterMovement()->MaxWalkSpeed = 0.f;
	UE_LOG(LogTemp, Warning, TEXT("Character is Groggy!"));
}

void ACharacterBase::TakeDamage(float Damage)
{
	Stat->ApplyDamage(Damage);
}

void ACharacterBase::SetupCharacterWidget(UMyPlayerUserWidget* UserWidget)
{
	UPlayerHpBar* PlayerHpBar = Cast<UPlayerHpBar>(UserWidget);

	if (PlayerHpBar)
	{
		PlayerHpBar->SetMaxHp(Stat->GetMaxHp());
		PlayerHpBar->UpdateHpBar(Stat->GetCurrentHp());
		Stat->OnHpChanged.AddUObject(PlayerHpBar, &UPlayerHpBar::UpdateHpBar);
	}
}

