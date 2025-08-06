// Fill out your copyright notice in the Description page of Project Settings.


#include "AIBaseCharacter.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Components/ShapeComponent.h"
#include "AbilitySystemGlobals.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AI/Controller/AIBaseController.h"

#include "../Attributes/AIBaseAttributeSet.h"
#include "Tags/TPTGameplayTags.h"
#include "Log/TPTLog.h"


AAIBaseCharacter::AAIBaseCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    AbilitySystem = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystem"));
    AttributeSet = CreateDefaultSubobject<UAIBaseAttributeSet>(TEXT("AttributeSet"));

    AutoPossessPlayer = EAutoReceiveInput::Disabled;
    AIControllerClass = nullptr;

    bUseControllerRotationYaw = false;
    GetCharacterMovement()->bOrientRotationToMovement = true;

    CombatRange = CreateDefaultSubobject<USphereComponent>(TEXT("CombatRangeSphereComponent"));
    CombatRange->SetupAttachment(RootComponent);
}

void AAIBaseCharacter::BeginPlay()
{
    Super::BeginPlay();

    if (!HasAuthority())
        return;

    //GetCharacterMovement()->MaxAcceleration = 800.f;           // 느린 가속
    //GetCharacterMovement()->BrakingDecelerationWalking = 600.f; // 느린 감속

    CombatRange->OnComponentBeginOverlap.AddDynamic(this, &AAIBaseCharacter::CombatRangeBeginOverlap);
    if (nullptr != AttackCollision)
    {
        AttackCollision->OnComponentBeginOverlap.AddDynamic(this, &AAIBaseCharacter::AttackCollisionBeginOverlap);
        SetAttackCollision(false);
    }
    else
        TPT_LOG(AILog, Warning, TEXT("If you want to AttackCollision, Please Set AttackCollision"));

    if (AbilitySystem)
    {
        AbilitySystem->InitAbilityActorInfo(this, this);
        AbilitySystem->AddReplicatedLooseGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_Identifier_AI);
        AbilitySystem->AddReplicatedLooseGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_Identifier_AI);

        for (const auto& Ability : Abilities)
        {
            if (!Ability.Key.IsValid() || !IsValid(Ability.Value))
                continue;

            const EFTPTGameplayTags* TagEnum = FTPTGameplayTags::Get().TagMap.Find(Ability.Key);
            int32 InputID = static_cast<int32>(*TagEnum);

            FGameplayAbilitySpec StartSpec(Ability.Value);
            StartSpec.InputID = InputID;
            AbilitySystem->GiveAbility(StartSpec);
        }

        AbilitySystem->RegisterGameplayTagEvent(FTPTGameplayTags::Get().TPTGameplay_Character_AIState_Stun)
            .AddUObject(this, &AAIBaseCharacter::ResetDataForState);
        AbilitySystem->RegisterGameplayTagEvent(FTPTGameplayTags::Get().TPTGameplay_Character_AIState_Default)
            .AddUObject(this, &AAIBaseCharacter::ResetDataForState);
        AbilitySystem->RegisterGameplayTagEvent(FTPTGameplayTags::Get().TPTGameplay_Character_AIState_Suspicion)
            .AddUObject(this, &AAIBaseCharacter::ResetDataForState);
        AbilitySystem->RegisterGameplayTagEvent(FTPTGameplayTags::Get().TPTGameplay_Character_AIState_Combat)
            .AddUObject(this, &AAIBaseCharacter::ResetDataForState);
    }
}

void AAIBaseCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

}
void AAIBaseCharacter::PostInitializeComponents()
{
    Super::PostInitializeComponents();
}

void AAIBaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

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

void AAIBaseCharacter::ResetDataForState(const FGameplayTag Tag, int32 TagCount)
{
    const EFTPTGameplayTags* EnumTag = FTPTGameplayTags::Get().TagMap.Find(Tag);
    NULLCHECK_RETURN_LOG(EnumTag, AILog, Warning, );

    if (TagCount > 0)
    {
        switch (*EnumTag)
        {
        case EFTPTGameplayTags::TPTGameplay_Character_AIState_Stun:
            ResetDataForStunState();
            break;
        case EFTPTGameplayTags::TPTGameplay_Character_AIState_Default:
            ResetDataForDefaultState();
            break;
        case EFTPTGameplayTags::TPTGameplay_Character_AIState_Suspicion:
            ResetDataForSuspicionState();
            break;
        case EFTPTGameplayTags::TPTGameplay_Character_AIState_Combat:
            ResetDataForCombatState();
            break;
        default:
            break;
        }
    }
    else // (TagCount <= 0)
    {
        switch (*EnumTag)
        {
        case EFTPTGameplayTags::TPTGameplay_Character_AIState_Stun:
            ResetDataForEscapeStunState();
            break;
        case EFTPTGameplayTags::TPTGameplay_Character_AIState_Default:
            ResetDataForEscapeDefaultState();
            break;
        case EFTPTGameplayTags::TPTGameplay_Character_AIState_Suspicion:
            ResetDataForEscapeSuspicionState();
            break;
        case EFTPTGameplayTags::TPTGameplay_Character_AIState_Combat:
            ResetDataForEscapeCombatState();
            break;
        default:
            break;
        }
    }
}

void AAIBaseCharacter::CombatRangeBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OtherActor);
    NULLCHECK_RETURN_LOG(ASC, AILog, Log, );

    // Player
    if (ASC->HasMatchingGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_Identifier_Player))
    {
        //SweepResult를 이용해서도 확인 가능, 만약 레이케스팅이 부적절하면 Sweep의 정보를 이용해서 사용
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
            AAIController* AIController = Cast<AAIController>(GetController());
            NULLCHECK_RETURN_LOG(AIController, AILog, Warning, );

            AAIBaseController* AIBaseController = Cast<AAIBaseController>(GetController());
            NULLCHECK_RETURN_LOG(AIBaseController, AILog, Warning, );

            UBlackboardComponent* BB = AIController->GetBlackboardComponent();
            NULLCHECK_RETURN_LOG(BB, AILog, Warning, );

            AIBaseController->AddPerceptionSightList(OtherActor);
            BB->SetValueAsBool(TEXT("bInCombatRange"), true);
            BB->SetValueAsFloat(TEXT("SightDuration"), TNumericLimits<float>::Max());
        }
        
    }
}

void AAIBaseCharacter::SetAttackCollision(bool bIsActive)
{
    NULLCHECK_RETURN_LOG(AttackCollision, AILog, Warning, );
    if (bIsActive)
    {
        AttackCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
        AttackCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
        AttackCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
        AttackCollision->SetCollisionResponseToChannel(ECC_Visibility, ECR_Overlap);
        //AttackCollision->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap);
        //AttackCollision->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
    }
    else
    {
        AttackCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }
}

UShapeComponent& AAIBaseCharacter::GetAttackCollision() const
{
    return *AttackCollision;
}

void AAIBaseCharacter::AttackCollisionBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (this == OtherActor)
        return;
    NULLCHECK_RETURN_LOG(DamageEffectClass, AILog, Log, );

    UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OtherActor);
    NULLCHECK_RETURN_LOG(TargetASC, AILog, Log, );

    UAbilitySystemComponent* SourceASC = GetAbilitySystemComponent(); 
    NULLCHECK_RETURN_LOG(SourceASC, AILog, Log, );

    FGameplayEffectContextHandle EffectContext = SourceASC->MakeEffectContext();
    EffectContext.AddSourceObject(this);

    FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass, 1.0f, EffectContext);
    if (SpecHandle.IsValid())
    {
        TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
    }

    AttackCollisionEvent(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}

void AAIBaseCharacter::ExcuteChaseActorGA(AActor* TargetActor)
{
    FGameplayEventData EventData;
    EventData.Instigator = this;
    EventData.Target = TargetActor;
    EventData.EventMagnitude = 1.f; // Will Set With Attribute
    AbilitySystem->HandleGameplayEvent(FTPTGameplayTags::Get().TPTGameplay_Data_Effect_AIChasing, &EventData);
}

void AAIBaseCharacter::ResetDataForStunState_Implementation()
{
    AAIController* AIController = Cast<AAIController>(GetController());
    NULLCHECK_RETURN_LOG(AIController, AILog, Warning, );
    UBlackboardComponent* BB = AIController->GetBlackboardComponent();
    NULLCHECK_RETURN_LOG(BB, AILog, Warning, );
}

void AAIBaseCharacter::ResetDataForDefaultState_Implementation()
{
    AAIController* AIController = Cast<AAIController>(GetController());
    NULLCHECK_RETURN_LOG(AIController, AILog, Warning, );
    UBlackboardComponent* BB = AIController->GetBlackboardComponent();
    NULLCHECK_RETURN_LOG(BB, AILog, Warning, );

    BB->SetValueAsBool("bInCombatRange", false);
    BB->SetValueAsObject("StimulusSplineActor", nullptr);
    const FVector InvalidLocation(FLT_MAX, FLT_MAX, FLT_MAX);
    BB->SetValueAsVector("StimulusLocation", InvalidLocation);
    BB->SetValueAsObject("TargetActor", nullptr);
    BB->SetValueAsFloat("SightDuration", 0.f);
    BB->SetValueAsFloat("LastSightTime", -1.f);
    BB->SetValueAsFloat("HearingSum", 0.f);
    BB->SetValueAsFloat("LastHearingTime", -1.f);
    BB->SetValueAsInt("Priority", std::numeric_limits<int32>::max());
}

void AAIBaseCharacter::ResetDataForSuspicionState_Implementation()
{
    AAIController* AIController = Cast<AAIController>(GetController());
    NULLCHECK_RETURN_LOG(AIController, AILog, Warning, );
    UBlackboardComponent* BB = AIController->GetBlackboardComponent();
    NULLCHECK_RETURN_LOG(BB, AILog, Warning, );
}

void AAIBaseCharacter::ResetDataForCombatState_Implementation()
{
    AAIController* AIController = Cast<AAIController>(GetController());
    NULLCHECK_RETURN_LOG(AIController, AILog, Warning, );
    UBlackboardComponent* BB = AIController->GetBlackboardComponent();
    NULLCHECK_RETURN_LOG(BB, AILog, Warning, );

    BB->SetValueAsFloat("SightDuration", std::numeric_limits<float>::max());
    BB->SetValueAsFloat("HearingSum", std::numeric_limits<float>::max());
    
    AActor* TargetActor = nullptr;
    UObject* Object = BB->GetValueAsObject("TargetActor");
    if (nullptr != Object)
        TargetActor = Cast<AActor>(Object);

    ExcuteChaseActorGA(TargetActor);
}

void AAIBaseCharacter::ResetDataForEscapeDefaultState_Implementation()
{
}

void AAIBaseCharacter::ResetDataForEscapeSuspicionState_Implementation()
{
}

void AAIBaseCharacter::ResetDataForEscapeStunState_Implementation()
{
}

void AAIBaseCharacter::ResetDataForEscapeCombatState_Implementation()
{
    AAIController* AIController = Cast<AAIController>(GetController());
    NULLCHECK_RETURN_LOG(AIController, AILog, Warning, );
    UBlackboardComponent* BB = AIController->GetBlackboardComponent();
    NULLCHECK_RETURN_LOG(BB, AILog, Warning, );

    BB->SetValueAsFloat("SightDuration", 0.f);
    BB->SetValueAsFloat("HearingSum", 0.f);
    BB->SetValueAsObject("TargetActor", nullptr);

    ExcuteChaseActorGA(nullptr);
}

