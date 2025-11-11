#include "InteractHideObject.h"
#include "Net/UnrealNetwork.h"
#include "Camera/CameraComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "../Player/PlayerCharacter.h"
#include "Player/PS_Player.h"
#include "Player/PC_Player.h"

#include "Materials/MaterialInterface.h"
#include "../Log/TPTLog.h"

#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISense_Hearing.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"

#include "Components/StaticMeshComponent.h"
#include "Tags/TPTGameplayTags.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/AudioComponent.h"
#include "Player/FocusTraceComponent.h"
#include "Components/WidgetComponent.h"

AInteractHideObject::AInteractHideObject() : AInteractableObject()
{
    // Camera
    HideCameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("HideCamComponent"));
    HideCameraComp->SetupAttachment(RootSceneComp);

    // AIPerception과 player안의 sphere만 감지하는 Object
    SphereCollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
    SphereCollisionComp->SetupAttachment(RootSceneComp);
    SphereCollisionComp->SetSphereRadius(50.0f);
    SphereCollisionComp->SetCollisionObjectType(ECC_GameTraceChannel1); // Object Type 설정

    /// AI Perception
    StimuliSource = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("StimuliSource"));
    StimuliSource->bAutoRegister = true;
    StimuliSource->RegisterForSense(UAISense_Sight::StaticClass());
    StimuliSource->RegisterForSense(UAISense_Hearing::StaticClass());

    // Box
    InPosBox = CreateDefaultSubobject<UBoxComponent>(TEXT("InPosBoxComp"));
    InPosBox->SetupAttachment(RootComponent);

    OutPosBox = CreateDefaultSubobject<UBoxComponent>(TEXT("OutPosBoxComp"));
    OutPosBox->SetupAttachment(RootComponent);

    // Lock
    LockWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("LockWidget"));
    LockWidgetComp->SetupAttachment(RootComponent);
    LockWidgetComp->SetWidgetSpace(EWidgetSpace::Screen);
    LockWidgetComp->SetDrawSize(FVector2D(10, 10));
    LockWidgetComp->SetRelativeLocation(FVector(0, 0, 100));
    LockWidgetComp->SetVisibility(false);

    NetPriority = 3.0f;
}

void AInteractHideObject::BeginPlay()
{
    Super::BeginPlay();

    if (LockWidgetComp)
    {
        if (LockWidgetClass)
        {
            LockWidgetComp->SetWidgetClass(LockWidgetClass);
            LockWidgetComp->SetVisibility(false);
        }
    }
}

void AInteractHideObject::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(AInteractHideObject, HidePlayer);
}

void AInteractHideObject::SetWidgetVisible(int32 bVisible)
{
    // 0 : 아무 위젯도 안보임
    // 1 : InteractWidget 보임
    // 2 : LockWidget 보임

    if (!InteractWidgetComp || !LockWidgetComp)
    {
        return;
    }

    switch (bVisible)
    {
    case 0:      // 0 : 아무 위젯도 안보임
        InteractWidgetComp->SetVisibility(false);
        LockWidgetComp->SetVisibility(false);
        return;

    case 1:      // 1 : InteractWidget 보임
        InteractWidgetComp->SetVisibility(true);
        LockWidgetComp->SetVisibility(false);
        return;

    case 2:      // 2 : LockWidget 보임
        InteractWidgetComp->SetVisibility(false);
        LockWidgetComp->SetVisibility(true);
        return;

    default:
        return;
    }
}

bool AInteractHideObject::CanInteract_Implementation(const APawn* Interactor, bool bIsDetected)
{
    //bCanInteract = bIsDetected;

    if (!Interactor->IsLocallyControlled())
    {
        return bIsDetected;
    }

    if (!bIsDetected)
    {
        SetWidgetVisible(0);
        return false;
    }
    else
    {
        // Interactor는 플레이어 캐릭터여야 함
        const APlayerCharacter* InteractPlayerChar = Cast<APlayerCharacter>(const_cast<APawn*>(Interactor));

        // 플레이어가 아니라면 모든 위젯 숨기고 상호작용 불가능
        if (!InteractPlayerChar)
        {
            SetWidgetVisible(0);
            return false;
        }
        else
        {
            // 숨은 플레이어가 없음 : 상호작용 가능
            if (!HidePlayer)
            {
                SetWidgetVisible(1);
                return true;
            }
            else
            {
                // 자신이 이미 숨은 상태일 때  : 상호작용 가능
                if (HidePlayer == InteractPlayerChar)
                {
                    SetWidgetVisible(0);
                    return true;
                }
                else
                {   // 다른 사람이 이미 들어가 있을 때 : 상호작용 불가능
                    SetWidgetVisible(2);
                    return true;
                }
            }
        }
    }
}

void AInteractHideObject::OnInteractServer_Implementation(const APawn* Interactor)
{
    // 서버에서 실행되는 코드
    if (!HasAuthority()) return;

    //if (HidePlayer != Interactor && HidePlayer != nullptr)
    //{
    //   PlayHideUnable(Interactor);
    //   return;
    //}

    TPT_LOG(ObjectLog, Log, TEXT("InteractHideObject::OnInteract Server"));

    if (nullptr == HidePlayer)
    {
        HidePlayer = const_cast<APawn*>(Interactor);
        EnterObject(Interactor);
        if (Interactor->IsLocallyControlled())
        {
            UKismetSystemLibrary::PrintString(this, TEXT("CamLogicClient"));
            CamLogicClient(Interactor);
        }
        else
        {
            UKismetSystemLibrary::PrintString(this, TEXT("S2C_CamLogicClient"));
            S2C_CamLogicClient(Interactor);
        }
        bIsActived = true;
    }
    else
    {
        if (HidePlayer == Interactor)
        {
            ExitObject();
            if (Interactor->IsLocallyControlled())
            {
                UKismetSystemLibrary::PrintString(this, TEXT("CamLogicClient"));
                CamLogicClient(Interactor);
            }
            else
            {
                UKismetSystemLibrary::PrintString(this, TEXT("CamLogicClient"));
                S2C_CamLogicClient(Interactor);
            }
            bIsActived = false;
            HidePlayer = nullptr;
        }
    }

    UpdateHideWidget();
}

void AInteractHideObject::OnInteractClient_Implementation(const APawn* Interactor)
{
    if (!Interactor->IsLocallyControlled()) return;

    TPT_LOG(ObjectLog, Log, TEXT("InteractHideObject::OnInteract Client"));

    if (HidePlayer != Interactor && HidePlayer != nullptr)
    {
        TPT_LOG(ObjectLog, Log, TEXT("Don't Try Hide!"));
        PlayHideUnable(Interactor);
        return;
    }

    //CamLogicClient(HidePlayer);
    APlayerController* InteracorPC = Cast<APlayerController>(Interactor->GetController());

    if (bIsActived)
    {
        if (HasAuthority())
        {
            EnableVignetteEffect(true);
            PlayHideInSound(Interactor, true);
        }
        else
        {
            SetInputState(InteracorPC, false);
            EnableVignetteEffect(false);
            PlayHideInSound(Interactor, false);

            FRotator NewRotation = OutPosBox->GetComponentRotation();
            InteracorPC->SetControlRotation(NewRotation);
        }
    }
    else
    {
        if (!HasAuthority())
        {
            SetInputState(InteracorPC, true);
            EnableVignetteEffect(true);
            PlayHideInSound(Interactor, true);
        }
        else
        {

            EnableVignetteEffect(false);
            PlayHideInSound(Interactor, false);
        }
    }

    SetWidgetVisible(0);
}

void AInteractHideObject::OnDestroy_Implementation(const APawn* Interactor)
{
    if (!HidePlayer)
    {
        return;
    }
    else
    {
        APlayerCharacter* Player = Cast<APlayerCharacter>(const_cast<APawn*>(Interactor));
        NULLCHECK_RETURN_LOG(Player, ObjectLog, Error, );
        UAbilitySystemComponent* ASC = Player->GetAbilitySystemComponent();
        NULLCHECK_RETURN_LOG(ASC, ObjectLog, Error, );

        if (ASC->HasMatchingGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_State_Hide))
        {
            ASC->RemoveLooseGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_State_Hide);
            ASC->RemoveReplicatedLooseGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_State_Hide);
        }

        ACharacter* Char = Cast<ACharacter>(HidePlayer);
        UCapsuleComponent* Capsule = Char->GetCapsuleComponent();
        Capsule->SetCollisionObjectType(ECC_Pawn);
    }

    // 서버에서만 NetMulticast 호출
    if (HasAuthority())
    {
        S2A_OnDestroy();
    }
}

void AInteractHideObject::S2A_OnDestroy_Implementation()
{
    if (!HidePlayer)
    {
        return;
    }

    APlayerCharacter* Player = Cast<APlayerCharacter>(HidePlayer);
    NULLCHECK_RETURN_LOG(Player, ObjectLog, Error, );
    UAbilitySystemComponent* ASC = Player->GetAbilitySystemComponent();
    NULLCHECK_RETURN_LOG(ASC, ObjectLog, Error, );
    APC_Player* InteractorPC = Cast<APC_Player>(Player->GetController());
    NULLCHECK_RETURN_LOG(InteractorPC, ObjectLog, Error, );

    if (ASC->HasMatchingGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_State_Hide))
    {
        ASC->RemoveLooseGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_State_Hide);
        ASC->RemoveReplicatedLooseGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_State_Hide);
    }

    // 로컬 컨트롤러에서만 입력/카메라 제어
    if (InteractorPC->IsLocalController())
    {
        // 클라이언트에게 입력 활성화 명령 전달
        SetInputState(InteractorPC, false);

        // 클라이언트에게 카메라 전환 명령 전달 (오브젝트 캠 -> 플레이어 캠)
        SetViewTarget(InteractorPC, HidePlayer);
    }

    // 플레이어 옷장 반대방향으로 바라보게 하기
    FRotator NewRotation = OutPosBox->GetComponentRotation();
    InteractorPC->SetControlRotation(NewRotation);
}

void AInteractHideObject::CamLogicClient(const APawn* Interactor)
{
    APlayerCharacter* Player = Cast<APlayerCharacter>(const_cast<APawn*>(Interactor));
    NULLCHECK_RETURN_LOG(Player, ObjectLog, Error, );
    UAbilitySystemComponent* ASC = Player->GetAbilitySystemComponent();
    NULLCHECK_RETURN_LOG(ASC, ObjectLog, Error, );
    APC_Player* InteractorPC = Cast<APC_Player>(Player->GetController());
    NULLCHECK_RETURN_LOG(InteractorPC, ObjectLog, Error, );

	/* if (!Interactor->IsLocallyControlled())
		 return;*/

    TPT_LOG(ObjectLog, Log, TEXT("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"));

    if (!bIsActived)
    {
        TPT_LOG(ObjectLog, Log, TEXT("bIsActived == false, 플레이어 안으로 가는 로직"));

        // 클라이언트에게 입력 비활성화 명령 전달
        SetInputState(InteractorPC, true);

        // 클라이언트에게 카메라 전환 명령 전달 (플레이어 캠 -> 오브젝트 캠)
        SetViewTarget(InteractorPC, this);

        if (HideTagGE)
        {
            FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
            EffectContext.AddSourceObject(this);

            FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(HideTagGE, 1.f, EffectContext);
            if (SpecHandle.IsValid())
            {
                ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
            }
        }
        else
        {
            // 예외처리: HideTagGE가 없을 때는 직접 태그 추가
            if (ASC)
            {
                ASC->AddLooseGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_State_Hide);
            }
        }
    }
    else
    {
        TPT_LOG(ObjectLog, Log, TEXT("bIsActived == true, 플레이어 밖으로 가는 로직"));


        // 클라이언트에게 입력 활성화 명령 전달
        SetInputState(InteractorPC, false);

        // 클라이언트에게 카메라 전환 명령 전달 (오브젝트 캠 -> 플레이어 캠)
        SetViewTarget(InteractorPC, HidePlayer);

        // 플레이어 옷장 반대방향으로 바라보게 하기
        FRotator NewRotation = OutPosBox->GetComponentRotation();
        InteractorPC->SetControlRotation(NewRotation);

        if (HideTagGE)
        {
            FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
            EffectContext.AddSourceObject(this);

            ASC->RemoveActiveGameplayEffectBySourceEffect(HideTagGE, ASC, 1);
        }
    }
}

void AInteractHideObject::S2C_CamLogicClient_Implementation(const APawn* Interactor)
{
    CamLogicClient(Interactor);
}

void AInteractHideObject::EnterObject(const APawn* Interactor)
{
    APlayerCharacter* Player = Cast<APlayerCharacter>(const_cast<APawn*>(Interactor));
    NULLCHECK_RETURN_LOG(Player, ObjectLog, Error, );
    UAbilitySystemComponent* ASC = Player->GetAbilitySystemComponent();
    NULLCHECK_RETURN_LOG(ASC, ObjectLog, Error, );

    UCapsuleComponent* Capsule = Player->GetCapsuleComponent();
    Capsule->SetCollisionObjectType(ECC_WorldDynamic);

    FVector playerLocation = { HidePlayer->GetActorLocation().X, HidePlayer->GetActorLocation().Y, 0 };
    S2A_PlayEffect(playerLocation);

    // 플레이어 위치가 InPosBox로 변경
    if (InPosBox && HidePlayer)
    {
        Player->GetFocusTrace()->SetStartOfsset(0.0f);

        FVector NewLocation = InPosBox->GetComponentLocation();
        FRotator NewRotation = InPosBox->GetComponentRotation();
        HidePlayer->SetActorLocationAndRotation(NewLocation, NewRotation);

        ShowOverlayOutline(false);
    }
}

void AInteractHideObject::ExitObject()
{
    if (OutPosBox && HidePlayer)
    {
        APlayerCharacter* Player = Cast<APlayerCharacter>(HidePlayer);
        NULLCHECK_RETURN_LOG(Player, ObjectLog, Error, );
        UAbilitySystemComponent* ASC = Player->GetAbilitySystemComponent();
        NULLCHECK_RETURN_LOG(ASC, ObjectLog, Error, );

        // 플레이어에 Hide 태그가 있다면
        if (ASC->HasMatchingGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_State_Hide))
        {
            // 플레이어 Hide 태그 제거
            ASC->RemoveLooseGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_State_Hide);
            ASC->RemoveReplicatedLooseGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_State_Hide);
        }

        ACharacter* Char = Cast<ACharacter>(HidePlayer);
        UCapsuleComponent* Capsule = Char->GetCapsuleComponent();
        Capsule->SetCollisionObjectType(ECC_Pawn);

        FVector NewLocation = OutPosBox->GetComponentLocation();
        FRotator NewRotation = OutPosBox->GetComponentRotation();
        HidePlayer->SetActorLocationAndRotation(NewLocation, NewRotation);
        HidePlayer->GetController()->SetControlRotation(NewRotation);

        S2A_PlayEffect(NewLocation);

        ShowOverlayOutline(true);
    }
}

void AInteractHideObject::SetInputState(APlayerController* InteractorPC, bool bIgnoreInput)
{
    if (InteractorPC)
    {
        InteractorPC->SetIgnoreMoveInput(bIgnoreInput);
        InteractorPC->SetIgnoreLookInput(bIgnoreInput);

        TPT_LOG(ObjectLog, Log, TEXT("Client: SetIgnoreInput called with value: %s"),
           bIgnoreInput ? TEXT("True") : TEXT("False"));
    }
}

void AInteractHideObject::SetViewTarget(APlayerController* InteractorPC, AActor* NewViewTarget)
{
    if (InteractorPC && NewViewTarget)
    {
        InteractorPC->SetViewTarget(NewViewTarget);

        //TPT_LOG(ObjectLog, Log, TEXT("Client: SetViewTarget called with actor: %s"),
        //   *NewViewTarget->GetName());
    }
}

void AInteractHideObject::PlayHideInSound(const APawn* Interactor, bool Visible)
{
    if (!HideInPlayerSoundCue) return;

    if (Visible)
    {
        if (!ActiveAudioComponent) // SoundCue는 클래스에 UPROPERTY로 선언되어 있어야 함
        {
            ActiveAudioComponent = UGameplayStatics::SpawnSoundAttached(HideInPlayerSoundCue, Interactor->GetRootComponent());
        }
    }
    else
    {
        if (ActiveAudioComponent) // SoundCue는 클래스에 UPROPERTY로 선언되어 있어야 함
        {
            ActiveAudioComponent->Stop();
            ActiveAudioComponent = nullptr;
        }
    }
}

void AInteractHideObject::S2A_PlayEffect_Implementation(FVector EffectLocation)
{
    PlayEffectLogic(EffectLocation);

    ShowOverlayOutline(bIsActived);
}


void AInteractHideObject::PlayEffectLogic_Implementation(FVector EffectLocation)
{

}

void AInteractHideObject::OnRep_bIsActived()
{
    Super::OnRep_bIsActived();

}

void AInteractHideObject::EnableVignetteEffect(bool bEnable)
{
    if (!HideCameraComp) return;
    if (!VignetteMaterial) return;

    if (bEnable)
    {
        HideCameraComp->PostProcessSettings.AddBlendable(VignetteMaterial, 1.0f);
    }
    else
    {
        HideCameraComp->PostProcessSettings.RemoveBlendable(VignetteMaterial);
    }
}

void AInteractHideObject::UpdateHideWidget()
{
    // HidePlayer가 NULL 로 바뀌었는데
    if (nullptr == HidePlayer)
    {
        // LOCK 위젯이 보인다
        if (LockWidgetComp->IsWidgetVisible())
        {
            // Interact 위젯 활성화
            InteractWidgetComp->SetVisibility(true);
            LockWidgetComp->SetVisibility(false);
        }
        else
        {
            InteractWidgetComp->SetVisibility(false);
            LockWidgetComp->SetVisibility(false);
        }
    }
    else // HidePlayer가 존재하는데
    {
        // Interact 위젯이 보인다
        if (InteractWidgetComp->IsWidgetVisible())
        {
            // LOCK 위젯 활성화
            LockWidgetComp->SetVisibility(true);
            InteractWidgetComp->SetVisibility(false);
        }
    }
}

void AInteractHideObject::OnRep_HidePlayer()
{
    UpdateHideWidget();
}
