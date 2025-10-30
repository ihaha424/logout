
#include "InteractHideObject.h"
#include "Net/UnrealNetwork.h"
#include "Camera/CameraComponent.h"
#include "NiagaraComponent.h" 
#include "Engine/World.h"
#include "TimerManager.h"
#include "../Player/PlayerCharacter.h"
#include "Player/PS_Player.h"

#include "Materials/MaterialInterface.h"
#include "../Log/TPTLog.h"

#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISense_Hearing.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"

#include "Components/StaticMeshComponent.h"
#include "SaveGame/SaveIDComponent.h"
#include "SaveGame/TPTSaveGameManager.h"
#include "Tags/TPTGameplayTags.h"

#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"
#include "Player/FocusTraceComponent.h"

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
}

void AInteractHideObject::BeginPlay()
{
	Super::BeginPlay();

}

void AInteractHideObject::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AInteractHideObject, HidePlayer);
}

void AInteractHideObject::S2A_OnDestroy_Implementation()
{
	if (!HidePlayer)
	{
		return;
	}

	// 플레이어 컨트롤러 가져오기
	APlayerController* InteractorPC = Cast<APlayerController>(HidePlayer->GetController());
	if (!InteractorPC) return;

	// PlayerState 가져옴
	APS_Player* PS = InteractorPC->GetPlayerState<APS_Player>();
	if (!PS) return;

	UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
	if (!ASC) return;

	// 플레이어에 Hide 태그가 있다면
	if (ASC->HasMatchingGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_State_Hide))
	{
		// 플레이어 Hide 태그 제거
		ASC->RemoveLooseGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_State_Hide);
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

void AInteractHideObject::OnDestroy_Implementation(const APawn* Interactor)
{
	if (!HidePlayer)
	{
		return;
	}

	// 서버에서만 NetMulticast 호출
	if (HasAuthority())
	{
		S2A_OnDestroy();
	}
}

bool AInteractHideObject::CanInteract_Implementation(const APawn* Interactor, bool bIsDetected)
{
	bCanInteract = bIsDetected;

	// 다른 사람이 이미 들어가 있을 때 return
	if (HidePlayer && HidePlayer != Interactor) 
	{
		if (Interactor->IsLocallyControlled())
		{
			SetWidgetVisible(false);
			PlayHideUnable(Interactor);
		}

		return false;
	}

	if (!Interactor->IsLocallyControlled()) return bCanInteract;

	//TPT_LOG(ObjectLog, Log,
	//	TEXT("InteractHideObject::CanInteract - %s | %s | Role: %s"),
	//	bCanInteract ? TEXT("true") : TEXT("false"),
	//	*Interactor->GetName(),
	//	*UEnum::GetValueAsString(GetLocalRole()));

	const APlayerCharacter* PlayerChar = Cast<APlayerCharacter>(const_cast<APawn*>(Interactor));
	if (PlayerChar)
	{
		SetWidgetVisible(bCanInteract);
	}

	return bCanInteract;
}

void AInteractHideObject::OnInteractServer_Implementation(const APawn* Interactor)
{
	// 서버에서 실행되는 코드
	if (!HasAuthority()) return;

	//TPT_LOG(ObjectLog, Log, TEXT("InteractHideObject::OnInteract Server"));
	PlayHideInSound(Interactor, true);

	CamLogicServer(Interactor);
}

void AInteractHideObject::OnInteractClient_Implementation(const APawn* Interactor)
{
	if (HasAuthority()) return;

	if (!Interactor->IsLocallyControlled()) return;

	//TPT_LOG(ObjectLog, Log, TEXT("InteractHideObject::OnInteract Client"));

	PlayHideInSound(Interactor, true);

	SetWidgetVisible(false);

	CamLogicClient(Interactor);
}

void AInteractHideObject::CamLogicServer(const APawn* Interactor)
{
	// 플레이어 컨트롤러 가져오기
	APlayerController* InteractorPC = CastChecked<APlayerController>(Interactor->GetController());
	const APlayerCharacter* Character = Cast<APlayerCharacter>(Interactor);
	NULLCHECK_RETURN_LOG(Character, ObjectLog, Warning, );
	UAbilitySystemComponent* ASC = Character->GetAbilitySystemComponent();
	NULLCHECK_RETURN_LOG(ASC, ObjectLog, Warning, );

	if (!bIsActived)	// 플레이어가 밖에 있는 경우(아직 활성화X)
	{
		EnterObject(Interactor);

		// 클라이언트에게 입력 비활성화 명령 전달
		SetInputState(InteractorPC, true);

		// 클라이언트에게 카메라 전환 명령 전달 (플레이어 캠 -> 오브젝트 캠)
		SetViewTarget(InteractorPC, this);

		EnableVignetteEffect(true);

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
	}
	else	// 플레이어가 안에 있는 경우(활성화O)
	{
		APawn* PlayerActor = HidePlayer;

		ExitObject();

		if (const APlayerCharacter* Player = Cast<APlayerCharacter>(Interactor))
		{
			Player->GetFocusTrace()->SetStartOfsset(100.0f);
		}

		// 클라이언트에게 입력 활성화 명령 전달
		SetInputState(InteractorPC, false);

		// 클라이언트에게 카메라 전환 명령 전달 (오브젝트 캠 -> 플레이어 캠)
		SetViewTarget(InteractorPC, PlayerActor);

		EnableVignetteEffect(false);

		PlayHideInSound(Interactor, false);

		if (HideTagGE)
		{
			FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
			EffectContext.AddSourceObject(this);

			ASC->RemoveActiveGameplayEffectBySourceEffect(HideTagGE, ASC, 1);
		}
	}
}

void AInteractHideObject::CamLogicClient(const APawn* Interactor)
{
	// 플레이어 컨트롤러 가져오기
	APlayerController* InteractorPC = CastChecked<APlayerController>(Interactor->GetController());
	const APlayerCharacter* Character = Cast<APlayerCharacter>(Interactor);
	NULLCHECK_RETURN_LOG(Character, ObjectLog, Warning, );
	UAbilitySystemComponent* ASC = Character->GetAbilitySystemComponent();
	NULLCHECK_RETURN_LOG(ASC, ObjectLog, Warning, );

	if (!bIsActived)
	{
		// 클라이언트에게 입력 비활성화 명령 전달
		SetInputState(InteractorPC, true);

		// 클라이언트에게 카메라 전환 명령 전달 (플레이어 캠 -> 오브젝트 캠)
		SetViewTarget(InteractorPC, this);

		EnableVignetteEffect(true);

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
	}
	else
	{
		// 클라이언트에게 입력 활성화 명령 전달
		SetInputState(InteractorPC, false);

		// 클라이언트에게 카메라 전환 명령 전달 (오브젝트 캠 -> 플레이어 캠)
		SetViewTarget(InteractorPC, HidePlayer);

		// 플레이어 옷장 반대방향으로 바라보게 하기
		FRotator NewRotation = OutPosBox->GetComponentRotation();
		InteractorPC->SetControlRotation(NewRotation);

		PlayHideInSound(Interactor, false);

		EnableVignetteEffect(false);

		if (HideTagGE)
		{
			FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
			EffectContext.AddSourceObject(this);

			ASC->RemoveActiveGameplayEffectBySourceEffect(HideTagGE, ASC, 1);
		}
	}
}

void AInteractHideObject::EnterObject(const APawn* Interactor)
{
	// PlayerState 가져옴
	APlayerController* InteractorPC = CastChecked<APlayerController>(Interactor->GetController());
	APS_Player* PS = InteractorPC->GetPlayerState<APS_Player>();
	UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();

	// 플레이어에 Hide 태그가 없다면
	if (!ASC->HasMatchingGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_State_Hide))
	{
		// 플레이어 Hide 태그 추가
		ASC->AddLooseGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_State_Hide);
	}

	bIsActived = true;
	HidePlayer = const_cast<APawn*>(Interactor);

	FVector playerLocation = { HidePlayer->GetActorLocation().X, HidePlayer->GetActorLocation().Y, 0 };
	S2A_PlayEffect(playerLocation);

	// 플레이어 위치가 InPosBox로 변경
	if (InPosBox && HidePlayer)
	{
		if (const APlayerCharacter* Player = Cast<APlayerCharacter>(Interactor))
		{
			Player->GetFocusTrace()->SetStartOfsset(0.0f);
		}

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
		// PlayerState 가져옴
		APlayerController* InteractorPC = CastChecked<APlayerController>(HidePlayer->GetController());
		APS_Player* PS = InteractorPC->GetPlayerState<APS_Player>();
		UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();

		// 플레이어에 Hide 태그가 있다면
		if (ASC->HasMatchingGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_State_Hide))
		{
			// 플레이어 Hide 태그 제거
			ASC->RemoveLooseGameplayTag(FTPTGameplayTags::Get().TPTGameplay_Character_State_Hide);
		}

		FVector NewLocation = OutPosBox->GetComponentLocation();
		FRotator NewRotation = OutPosBox->GetComponentRotation();
		HidePlayer->SetActorLocationAndRotation(NewLocation, NewRotation);
		InteractorPC->SetControlRotation(NewRotation);

		S2A_PlayEffect(NewLocation);

		ShowOverlayOutline(true);
	}


	bIsActived = false;
	HidePlayer = nullptr;
}

void AInteractHideObject::SetInputState(APlayerController* InteractorPC, bool bIgnoreInput)
{
	if (InteractorPC && InteractorPC->IsLocalController())
	{
		InteractorPC->SetIgnoreMoveInput(bIgnoreInput);
		InteractorPC->SetIgnoreLookInput(bIgnoreInput);

		//TPT_LOG(ObjectLog, Log, TEXT("Client: SetIgnoreInput called with value: %s"),
		//	bIgnoreInput ? TEXT("True") : TEXT("False"));
	}
}

void AInteractHideObject::SetViewTarget(APlayerController* InteractorPC, AActor* NewViewTarget)
{
	if (!InteractorPC->IsLocalController()) return;

	if (InteractorPC && NewViewTarget)
	{
		InteractorPC->SetViewTarget(NewViewTarget);

		//TPT_LOG(ObjectLog, Log, TEXT("Client: SetViewTarget called with actor: %s"),
		//	*NewViewTarget->GetName());
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
