// Fill out your copyright notice in the Description page of Project Settings.


#include "AIBaseCharacter.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SphereComponent.h"
#include "AbilitySystemGlobals.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"

#include "../Attributes/AIBaseAttributeSet.h"
#include "Tags/TPTGameplayTags.h"
#include "Log/TPTLog.h"


AAIBaseCharacter::AAIBaseCharacter()
{
    PrimaryActorTick.bCanEverTick = false;

    AbilitySystem = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystem"));
    AttributeSet = CreateDefaultSubobject<UAIBaseAttributeSet>(TEXT("AttributeSet"));

    AutoPossessPlayer = EAutoReceiveInput::Disabled;
    AIControllerClass = nullptr;

    bUseControllerRotationYaw = false;
    GetCharacterMovement()->bOrientRotationToMovement = true;

    CombatRange = CreateDefaultSubobject<USphereComponent>(TEXT("CombatRangeSphereComponent"));
    CombatRange->SetupAttachment(RootComponent);
    CombatRange->OnComponentBeginOverlap.AddDynamic(this, &AAIBaseCharacter::CombatRangeBeginOverlap);

}

void AAIBaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
    if (AbilitySystem)
    {
        AbilitySystem->InitAbilityActorInfo(this, this);
        AbilitySystem->AddLooseGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_Identifier_AI);

        for (const auto& Ability : Abilities)
        {
            const EFTPTGameplayTags* TagEnum = FTPTGameplayTags::Get().TagMap.Find(Ability.Key);
            int32 InputID = static_cast<int32>(*TagEnum);

            FGameplayAbilitySpec StartSpec(Ability.Value);
            StartSpec.InputID = InputID;
            AbilitySystem->GiveAbility(StartSpec);
        }

        // TODO: 상태 변화시 함수 바인딩 -> 바인된 함수는 상태에 따른 효과 제공
        //AbilitySystem->RegisterGameplayTagEvent(CombatTag, EGameplayTagEventType::NewOrRemoved)
        //        .AddUObject(this, &UAIStateEffectComponent::OnCombatStateChanged);
        //AbilitySystem->RegisterGameplayTagEvent(StunnedTag, EGameplayTagEventType::NewOrRemoved)
        //        .AddUObject(this, &UAIStateEffectComponent::OnStunnedStateChanged);
        
    }
}

UAbilitySystemComponent* AAIBaseCharacter::GetAbilitySystemComponent() const
{
    return AbilitySystem;
}

UAIBaseAttributeSet* AAIBaseCharacter::GetAIAttributeSet() const
{
    return AttributeSet;
}

void AAIBaseCharacter::ApplyStun()
{
    AIStateTags.Reset();
    AIStateTags.AddTag(FTPTGameplayTags::Get().TPTGameplay_Character_AIState_Stun);

    // TODO: 움직임 멈추기, 몽타주 중단, 이펙트 재생 등
}

void AAIBaseCharacter::ResetToDefaultState()
{
    AIStateTags.Reset();
    AIStateTags.AddTag(FTPTGameplayTags::Get().TPTGameplay_Character_AIState_Default);

    // TODO: 원래 위치로 이동하거나 순찰 재시작
}

FString AAIBaseCharacter::GetCurrentAIStateAsString() const
{
    return AIStateTags.ToStringSimple();
}

void AAIBaseCharacter::CombatRangeBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    TPT_LOG(AILog, Error, TEXT("CombatRangeBeginOverlap"));
    UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OtherActor);
    NULLCHECK_RETURN_LOG(ASC, AILog, Log, );
    // Player
    TPT_LOG(AILog, Error, TEXT("CombatRangeBeginOverlapASC"));
    if (ASC->HasMatchingGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_Identifier_Player))
    {
        AAIController* AIController = Cast<AAIController>(GetController());
        NULLCHECK_RETURN_LOG(AIController, AILog, Warning, );
        UBlackboardComponent* BB = AIController->GetBlackboardComponent();
        NULLCHECK_RETURN_LOG(BB, AILog, Warning, );


        FVector MyLoc = GetActorLocation();
        FVector TargetLoc = OtherActor->GetActorLocation();
        FHitResult HitResult;
        FCollisionQueryParams Params;
        Params.AddIgnoredActor(this);

        bool bHit = GetWorld()->LineTraceSingleByChannel(
            HitResult,
            MyLoc,
            TargetLoc,
            ECC_Pawn,
            Params
        );
#if WITH_EDITOR
        DrawDebugLine(
            GetWorld(),
            MyLoc,
            TargetLoc,
            FColor::Red,
            false,
            2.0f,
            0,
            2.0f
        );
#endif
        if (bHit && HitResult.GetActor() == OtherActor)
        {
            AActor* TargetActor = Cast<AActor>(BB->GetValueAsObject(TEXT("TargetActor")));
            if (nullptr != TargetActor && TargetActor != OtherActor)
            {
                float NewDistance = FVector::Dist(MyLoc, TargetLoc);

                TargetLoc = TargetActor->GetActorLocation();
                float CurDistance = FVector::Dist(MyLoc, TargetLoc);

                if (NewDistance < CurDistance)
                    TargetActor = OtherActor;
                BB->SetValueAsObject(TEXT("TargetActor"), TargetActor);
            }
            else
                BB->SetValueAsObject(TEXT("TargetActor"), OtherActor);
            BB->SetValueAsBool(TEXT("bInCombatRange"), true);
            BB->SetValueAsFloat(TEXT("SightDuration"), TNumericLimits<float>::Max());
        }
        
    }
}

void AAIBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AAIBaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

