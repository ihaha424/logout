// Fill out your copyright notice in the Description page of Project Settings.


#include "AIBaseCharacter.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Components/ShapeComponent.h"
#include "Components/CapsuleComponent.h"
#include "AbilitySystemGlobals.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AI/Controller/AIBaseController.h"
#include "GameplayCueInterface.h"
#include "BrainComponent.h"

#include "../Attributes/AIBaseAttributeSet.h"
#include "Tags/TPTGameplayTags.h"
#include "Log/TPTLog.h"



AAIBaseCharacter::AAIBaseCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    AbilitySystem = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystem"));

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
    GetCharacterMovement()->MaxWalkSpeed = MoveSpeed;

    CombatRange->OnComponentBeginOverlap.AddDynamic(this, &AAIBaseCharacter::CombatRangeBeginOverlap);
    CombatRange->OnComponentEndOverlap.AddDynamic(this, &AAIBaseCharacter::CombatRangeEndOverlap);
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
        //AttributeSet = AbilitySystem->GetSet<UAIBaseAttributeSet>();
        //NULLCHECK_RETURN_LOG(AttributeSet, AILog, Error, );
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

        for (const auto& CueNotify : GamePlayCueNotifies)
        {
            // If you Want to Cue Register, Here.
        }
        
        AbilitySystem->RegisterGameplayTagEvent(FTPTGameplayTags::Get().TPTGameplay_Character_AIState_Die)
            .AddUObject(this, &AAIBaseCharacter::ResetDataForState);
        AbilitySystem->RegisterGameplayTagEvent(FTPTGameplayTags::Get().TPTGameplay_Character_AIState_Stun)
            .AddUObject(this, &AAIBaseCharacter::ResetDataForState);
        AbilitySystem->RegisterGameplayTagEvent(FTPTGameplayTags::Get().TPTGameplay_Character_AIState_Default)
            .AddUObject(this, &AAIBaseCharacter::ResetDataForState);
        AbilitySystem->RegisterGameplayTagEvent(FTPTGameplayTags::Get().TPTGameplay_Character_AIState_Suspicion)
            .AddUObject(this, &AAIBaseCharacter::ResetDataForState);
        AbilitySystem->RegisterGameplayTagEvent(FTPTGameplayTags::Get().TPTGameplay_Character_AIState_Combat)
            .AddUObject(this, &AAIBaseCharacter::ResetDataForState);
        ResetDataForState(FTPTGameplayTags::Get().TPTGameplay_Character_AIState_Default, 1); // Initialize
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

const UAIBaseAttributeSet* AAIBaseCharacter::GetAIAttributeSet() const
{
    return AttributeSet;
}

void AAIBaseCharacter::ApplyStun_Implementation()
{
    AAIController* AIController = Cast<AAIController>(GetController());
    NULLCHECK_RETURN_LOG(AIController, AILog, Warning, );
    UBlackboardComponent* BB = AIController->GetBlackboardComponent();
    NULLCHECK_RETURN_LOG(BB, AILog, Warning, );

    BB->SetValueAsBool("bIsStunned", true);
}

void AAIBaseCharacter::ApplyDie_Implementation()
{
    AAIController* AIController = Cast<AAIController>(GetController());
    NULLCHECK_RETURN_LOG(AIController, AILog, Warning, );
    UBlackboardComponent* BB = AIController->GetBlackboardComponent();
    NULLCHECK_RETURN_LOG(BB, AILog, Warning, );

    BB->SetValueAsBool("bIsDie", true);
}

void AAIBaseCharacter::ApplyDestroy_Implementation()
{
    if (GetLocalRole() != ROLE_Authority)
        return;

    // GAS 정리: 능력/이펙트/큐 강제 종료
    if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(this))
    {
        ASC->CancelAllAbilities();        // 활성 GA 전부 취소
        ASC->RemoveAllGameplayCues();     // 활성 GameplayCue 제거

        // 활성 GE 전부 제거
        FGameplayEffectQuery Q;
        Q.CustomMatchDelegate.BindLambda([](const FActiveGameplayEffect&) { return true; });
        const int32 Removed = ASC->RemoveActiveEffects(Q);
    }

    // BT/AI 정지
    AAIController* AIController = Cast<AAIController>(GetController());
    if (AIController)
    {
        if (AIController->BrainComponent)
        { 
            AIController->BrainComponent->StopLogic(TEXT("KillNow"));
        }
    }
    DetachFromControllerPendingDestroy();

    // 파괴(실패 대비 Lifespan 백업)
    if (!Destroy())
    {
       SetLifeSpan(0.01f);
    }
}

void AAIBaseCharacter::ResetDataForState(const FGameplayTag Tag, int32 TagCount)
{
    S2A_ResetDataForState(Tag, TagCount);
}


void AAIBaseCharacter::S2A_ResetDataForState_Implementation(const FGameplayTag Tag, int32 TagCount)
{
    const TMap<EFTPTGameplayTags, EAIBaseState> StateMap =
    {
        { EFTPTGameplayTags::TPTGameplay_Character_AIState_Die, EAIBaseState::Die},
        { EFTPTGameplayTags::TPTGameplay_Character_AIState_Stun, EAIBaseState::Stun},
        { EFTPTGameplayTags::TPTGameplay_Character_AIState_Default, EAIBaseState::Default},
        { EFTPTGameplayTags::TPTGameplay_Character_AIState_Suspicion, EAIBaseState::Suspicion},
        { EFTPTGameplayTags::TPTGameplay_Character_AIState_Combat, EAIBaseState::Combat}
    };


    const EFTPTGameplayTags* EnumTag = FTPTGameplayTags::Get().TagMap.Find(Tag);
    NULLCHECK_RETURN_LOG(EnumTag, AILog, Warning, );

    bool bHandled = false;
    if (const EAIBaseState* State = StateMap.Find(*EnumTag))
    {
        if (TagCount > 0)
            ResetDataForEnterState(*State);
        else
            ResetDataForExitState(*State);

        bHandled = true;
    }
}

bool AAIBaseCharacter::MatchingChaseActorType(AActor* OtherActor) const
{
    UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OtherActor);
    if (nullptr == ASC)
        return false;

    return ASC->HasMatchingGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_Identifier_Player) && !ASC->HasMatchingGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_State_Downed);
}

void AAIBaseCharacter::CombatRangeBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor == this)
        return;
    if (this->MatchingChaseActorType(OtherActor))
    {
        if (INDEX_NONE == CombatRangeInActor.Find(OtherActor))
            CombatRangeInActor.Add(OtherActor);
        if (!CombatRangeInActorTimerHandle.IsValid())
        {
            GetWorld()->GetTimerManager().SetTimer(
                CombatRangeInActorTimerHandle,
                this,
                &AAIBaseCharacter::CheckCombatRangeInActor,
                0.2f,
                true
            );
        }
    }
}

void AAIBaseCharacter::CombatRangeEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    CombatRangeInActor.Remove(OtherActor);
    if (CombatRangeInActor.Num() < 1 && CombatRangeInActorTimerHandle.IsValid())
        GetWorld()->GetTimerManager().ClearTimer(CombatRangeInActorTimerHandle);
}

void AAIBaseCharacter::CheckCombatRangeInActor()
{
    for (AActor* actor : CombatRangeInActor)
    {
        if (!this->MatchingChaseActorType(actor))
            continue;
        //SweepResult를 이용해서도 확인 가능, 만약 레이케스팅이 부적절하면 Sweep의 정보를 이용해서 사용
        FVector MyLoc = GetActorLocation();
        FVector TargetLoc = actor->GetActorLocation();

        FHitResult HitResult;
        FCollisionQueryParams Params;
        Params.AddIgnoredActor(this);

        FCollisionObjectQueryParams ObjParams;
        ObjParams.AddObjectTypesToQuery(ECC_GameTraceChannel1);
        ObjParams.AddObjectTypesToQuery(ECC_Pawn);
        ObjParams.AddObjectTypesToQuery(ECC_Visibility);

        const bool bHit = GetWorld()->LineTraceSingleByObjectType(
            HitResult, MyLoc, TargetLoc, ObjParams, Params
        );

    #if WITH_EDITOR
        //DrawDebugLine(
        //    GetWorld(),
        //    MyLoc,
        //    TargetLoc,
        //    FColor::Red,
        //    false,
        //    2.0f,
        //    0,
        //    2.0f
        //);
    #endif
        if (bHit && HitResult.GetActor() == actor)
        {
            AAIController* AIController = Cast<AAIController>(GetController());
            NULLCHECK_RETURN_LOG(AIController, AILog, Warning, );

            AAIBaseController* AIBaseController = Cast<AAIBaseController>(GetController());
            NULLCHECK_RETURN_LOG(AIBaseController, AILog, Warning, );

            UBlackboardComponent* BB = AIController->GetBlackboardComponent();
            NULLCHECK_RETURN_LOG(BB, AILog, Warning, );

            AIBaseController->AddPerceptionSightList(actor);
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
        AAIController* AIController = Cast<AAIController>(GetController());
        NULLCHECK_RETURN_LOG(AIController, AILog, Warning, );
        AIController->StopMovement();

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
    if (!SpecHandle.IsValid())
        return;
    SpecHandle.Data->SetContext(EffectContext);
    SpecHandle.Data->SetSetByCallerMagnitude(FTPTGameplayTags::Get().TPTGameplay_Data_Effect_MentalPoint, -AttackValue);
    SpecHandle.Data->SetSetByCallerMagnitude(FTPTGameplayTags::Get().TPTGameplay_Data_Effect_HP, -AttackValue);
    TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

    AttackCollisionEvent(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}

void AAIBaseCharacter::ExcuteChaseActorGA(AActor* TargetActor)
{
    FGameplayEventData EventData;
    EventData.Instigator = this;
    EventData.Target = TargetActor;
    EventData.EventMagnitude = -ChaseMentalAttackValue; // Will Set With Attribute
    AbilitySystem->HandleGameplayEvent(FTPTGameplayTags::Get().TPTGameplay_Data_Effect_AIChasing, &EventData);
}

void AAIBaseCharacter::CancleChaseActorGA()
{
    FGameplayTagContainer CancelTags;
    CancelTags.AddTag(FTPTGameplayTags::Get().TPTGameplay_Data_Effect_AIChasing);

    AbilitySystem->CancelAbilities(&CancelTags);
}

void AAIBaseCharacter::ResetDataForDieState_Implementation()
{
    if (!HasAuthority())
        return;

    AAIController* AIController = Cast<AAIController>(GetController());
    NULLCHECK_RETURN_LOG(AIController, AILog, Warning, );

    AIController->StopMovement();

    UCapsuleComponent* Capsule = GetCapsuleComponent();
    NULLCHECK_RETURN_LOG(Capsule, AILog, Warning, );

    Capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    Capsule->SetGenerateOverlapEvents(false);
    Capsule->SetNotifyRigidBodyCollision(false);

    USkeletalMeshComponent* ChracterMesh = GetMesh();
    NULLCHECK_RETURN_LOG(ChracterMesh, AILog, Warning, );

    ChracterMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    ChracterMesh->SetGenerateOverlapEvents(false);
    ChracterMesh->SetNotifyRigidBodyCollision(false);
}

void AAIBaseCharacter::ResetDataForStunState_Implementation()
{
    if (!HasAuthority())
        return;

    AAIController* AIController = Cast<AAIController>(GetController());
    NULLCHECK_RETURN_LOG(AIController, AILog, Warning, );

    AIController->StopMovement();
}

void AAIBaseCharacter::ResetDataForDefaultState_Implementation()
{
    if (!HasAuthority())
        return;

    AAIBaseController* AIController = Cast<AAIBaseController>(GetController());
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
    AIController->ResetSightList();
}

void AAIBaseCharacter::ResetDataForSuspicionState_Implementation()
{
    if (!HasAuthority())
        return;
}

void AAIBaseCharacter::ResetDataForCombatState_Implementation()
{
    if (!HasAuthority())
        return;

    AAIController* AIController = Cast<AAIController>(GetController());
    NULLCHECK_RETURN_LOG(AIController, AILog, Warning, );
    UBlackboardComponent* BB = AIController->GetBlackboardComponent();
    NULLCHECK_RETURN_LOG(BB, AILog, Warning, );

    BB->SetValueAsFloat("SightDuration", std::numeric_limits<float>::max());
    BB->SetValueAsFloat("HearingSum", std::numeric_limits<float>::max());

    CurSpeed = ChaseSpeed;
    GetCharacterMovement()->MaxWalkSpeed = CurSpeed;

    AActor* TargetActor = nullptr;
    UObject* Object = BB->GetValueAsObject("TargetActor");
    if (nullptr != Object)
        TargetActor = Cast<AActor>(Object);

    CancleChaseActorGA();
    ExcuteChaseActorGA(TargetActor);
}

void AAIBaseCharacter::ResetDataForEscapeDefaultState_Implementation()
{
    if (!HasAuthority())
        return;
}

void AAIBaseCharacter::ResetDataForEscapeSuspicionState_Implementation()
{
    if (!HasAuthority())
        return;
}

void AAIBaseCharacter::ResetDataForEscapeDieState_Implementation()
{
    if (!HasAuthority())
        return;
}

void AAIBaseCharacter::ResetDataForEscapeStunState_Implementation()
{
    if (!HasAuthority())
        return;
}

void AAIBaseCharacter::ResetDataForEscapeCombatState_Implementation()
{
    if (!HasAuthority())
        return;

    AAIController* AIController = Cast<AAIController>(GetController());
    NULLCHECK_RETURN_LOG(AIController, AILog, Warning, );
    UBlackboardComponent* BB = AIController->GetBlackboardComponent();
    NULLCHECK_RETURN_LOG(BB, AILog, Warning, );

    BB->SetValueAsFloat("SightDuration", 0.f);
    BB->SetValueAsFloat("HearingSum", 0.f);
    BB->SetValueAsObject("TargetActor", nullptr);
    CurSpeed = MoveSpeed;
    GetCharacterMovement()->MaxWalkSpeed = CurSpeed;
    SetAttackCollision(false);
    CancleChaseActorGA();
}

void AAIBaseCharacter::ResetDataForEnterState_Implementation(EAIBaseState state)
{
    switch (state)
    {
    case EAIBaseState::Die:
        ResetDataForDieState();
        break;
    case EAIBaseState::Stun:
        ResetDataForStunState();
        break;
    case EAIBaseState::Default:
        ResetDataForDefaultState();
        break;
    case EAIBaseState::Suspicion:
        ResetDataForSuspicionState();
        break;
    case EAIBaseState::Combat:
        ResetDataForCombatState();
        break;
    default:
        break;
    }
}

void AAIBaseCharacter::ResetDataForExitState_Implementation(EAIBaseState state)
{
    switch (state)
    {
    case EAIBaseState::Die:
        ResetDataForEscapeDieState();
        break;
    case EAIBaseState::Stun:
        ResetDataForEscapeStunState();
        break;
    case EAIBaseState::Default:
        ResetDataForEscapeDefaultState();
        break;
    case EAIBaseState::Suspicion:
        ResetDataForEscapeSuspicionState();
        break;
    case EAIBaseState::Combat:
        ResetDataForEscapeCombatState();
        break;
    default:
        break;
    }
}
