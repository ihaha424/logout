// Fill out your copyright notice in the Description page of Project Settings.


#include "AIBaseCharacter.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Components/ShapeComponent.h"
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
}

void AAIBaseCharacter::BeginPlay()
{
    Super::BeginPlay();

    if (!HasAuthority())
        return;

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
        AbilitySystem->AddLooseGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_Identifier_AI);

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

        // TODO: 상태 변화시 함수 바인딩 -> 바인된 함수는 상태에 따른 효과 제공
        //AbilitySystem->RegisterGameplayTagEvent(CombatTag, EGameplayTagEventType::NewOrRemoved)
        //        .AddUObject(this, &UAIStateEffectComponent::OnCombatStateChanged);
        //AbilitySystem->RegisterGameplayTagEvent(StunnedTag, EGameplayTagEventType::NewOrRemoved)
        //        .AddUObject(this, &UAIStateEffectComponent::OnStunnedStateChanged);
        
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

void AAIBaseCharacter::CombatRangeBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OtherActor);
    NULLCHECK_RETURN_LOG(ASC, AILog, Log, );

    // Player
    if (ASC->HasMatchingGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_Identifier_Player))
    {
        AAIController* AIController = Cast<AAIController>(GetController());
        NULLCHECK_RETURN_LOG(AIController, AILog, Warning, );
        UBlackboardComponent* BB = AIController->GetBlackboardComponent();
        NULLCHECK_RETURN_LOG(BB, AILog, Warning, );

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


