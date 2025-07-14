
#include "InteractHideObject.h"
#include "Net/UnrealNetwork.h"
#include "Camera/CameraComponent.h"
#include "NiagaraComponent.h" 
#include "Engine/World.h"
#include "TimerManager.h"

#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISense_Hearing.h"
#include "Perception/AISenseConfig_Hearing.h"


AInteractHideObject::AInteractHideObject() : AInteractableObject()
{
	// Camera
	HideCameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("HideCamComponent"));
	HideCameraComp->SetupAttachment(RootSceneComp);

	// Effect
	HideEffectComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("HideEffectComponent"));
	HideEffectComp->SetupAttachment(RootComponent);

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

	if (HideEffectComp)
	{
		HideEffectComp->SetActive(false);
		HideEffectComp->SetVisibility(false);
	}
}

void AInteractHideObject::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AInteractHideObject, bIsInHideView);
	DOREPLIFETIME(AInteractHideObject, HidePlayer);
}

bool AInteractHideObject::CanInteract_Implementation(const APawn* Interactor, bool bIsDetected)
{
	// 다른 사람이 이미 들어가 있을 때 return
	if (HidePlayer && HidePlayer != Interactor) return false;

	if (!Interactor->IsLocallyControlled()) return false;

	bCanInteract = bIsDetected;

	UE_LOG(LogTemp, Log,
		TEXT("InteractHideObject::CanInteract - %s | %s | Role: %s"),
		bCanInteract ? TEXT("true") : TEXT("false"),
		*Interactor->GetName(),
		*UEnum::GetValueAsString(GetLocalRole()));

	SetWidgetVisible(bCanInteract);

	return bCanInteract;
}

void AInteractHideObject::OnInteractServer_Implementation(const APawn* Interactor)
{
	UE_LOG(LogTemp, Warning, TEXT("InteractHideObject::OnInteractServer"));

	//if (HidePlayer && HidePlayer != Interactor) return;

	// 플레이어 컨트롤러 가져오기
	APlayerController* PlayerController = CastChecked<APlayerController>(Interactor->GetController());
	
	S2A_PlayEffect(PlayerController);

	CamLogicServer(PlayerController);
}

void AInteractHideObject::OnInteractClient_Implementation(const APawn* Interactor)
{
	if (HasAuthority()) return;

	if (!Interactor->IsLocallyControlled()) return;

	SetWidgetVisible(false);

	// 플레이어 컨트롤러 가져오기
	APlayerController* PlayerController = CastChecked<APlayerController>(Interactor->GetController());
	CamLogicClient(PlayerController);
}

void AInteractHideObject::CamLogicServer(APlayerController* InteractorPC)
{
	// 서버에서 실행되는 코드
	if (!HasAuthority()) return;

	if (!bIsInHideView)
	{
		EnterObject(InteractorPC);

		// 클라이언트에게 입력 비활성화 명령 전달
		SetInputState(InteractorPC, true);

		// 클라이언트에게 카메라 전환 명령 전달 (플레이어 캠 -> 오브젝트 캠)
		SetViewTarget(InteractorPC, this);
	}
	else
	{
		ExitObject(InteractorPC);

		// 클라이언트에게 입력 활성화 명령 전달
		SetInputState(InteractorPC, false);

		// 클라이언트에게 카메라 전환 명령 전달 (오브젝트 캠 -> 플레이어 캠)
		SetViewTarget(InteractorPC, HidePlayer);
	}
}

void AInteractHideObject::CamLogicClient(APlayerController* InteractorPC)
{
	if (!bIsInHideView)
	{
		// 클라이언트에게 입력 비활성화 명령 전달
		SetInputState(InteractorPC, true);

		// 클라이언트에게 카메라 전환 명령 전달 (플레이어 캠 -> 오브젝트 캠)
		SetViewTarget(InteractorPC, this);
	}
	else
	{
		// 클라이언트에게 입력 활성화 명령 전달
		SetInputState(InteractorPC, false);

		// 클라이언트에게 카메라 전환 명령 전달 (오브젝트 캠 -> 플레이어 캠)
		SetViewTarget(InteractorPC, HidePlayer);
	}
}

void AInteractHideObject::EnterObject(APlayerController* InteractorPC)
{
	bIsInHideView = true;
	HidePlayer = InteractorPC->GetPawn();

	// 플레이어 위치가 InPosBox로 변경
	if (InPosBox && HidePlayer)
	{
		FVector NewLocation = InPosBox->GetComponentLocation();
		FRotator NewRotation = InPosBox->GetComponentRotation();
		HidePlayer->SetActorLocationAndRotation(NewLocation, NewRotation);
	}
}

void AInteractHideObject::ExitObject(APlayerController* InteractorPC)
{
	if (OutPosBox && HidePlayer)
	{
		FVector NewLocation = OutPosBox->GetComponentLocation();
		FRotator NewRotation = OutPosBox->GetComponentRotation();
		HidePlayer->SetActorLocationAndRotation(NewLocation, NewRotation);
	}

	bIsInHideView = false;
	HidePlayer = nullptr;
}

void AInteractHideObject::SetInputState(APlayerController* InteractorPC, bool bIgnoreInput)
{
	if (InteractorPC && InteractorPC->IsLocalController())
	{
		InteractorPC->SetIgnoreMoveInput(bIgnoreInput);
		InteractorPC->SetIgnoreLookInput(bIgnoreInput);

		UE_LOG(LogTemp, Log, TEXT("Client: SetIgnoreInput called with value: %s"),
			bIgnoreInput ? TEXT("True") : TEXT("False"));
	}
}

void AInteractHideObject::SetViewTarget(APlayerController* InteractorPC, AActor* NewViewTarget)
{
	if (!InteractorPC->IsLocalController()) return;

	if (InteractorPC && NewViewTarget)
	{
		InteractorPC->SetViewTarget(NewViewTarget);

		UE_LOG(LogTemp, Log, TEXT("Client: SetViewTarget called with actor: %s"),
			*NewViewTarget->GetName());
	}
}

void AInteractHideObject::S2A_PlayEffect_Implementation(APlayerController* InteractorPC)
{
	if (HideEffectComp)
	{
		// 이펙트 활성화 및 보이게 설정
		HideEffectComp->SetActive(true);
		HideEffectComp->SetVisibility(true);

		// 3초 후에 이펙트 비활성화 및 숨기기
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(
			TimerHandle,
			this,
			&AInteractHideObject::OnEffectFinished,
			3.0f,
			false
		);
	}
}

void AInteractHideObject::OnEffectFinished()
{
	if (HideEffectComp)
	{
		HideEffectComp->SetActive(false);
		HideEffectComp->SetVisibility(false);
	}
}