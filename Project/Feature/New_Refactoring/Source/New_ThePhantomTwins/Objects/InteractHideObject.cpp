
#include "InteractHideObject.h"
#include "Net/UnrealNetwork.h"
#include "Camera/CameraComponent.h"
#include "NiagaraComponent.h" 
#include "Engine/World.h"
#include "TimerManager.h"

AInteractHideObject::AInteractHideObject() : ABaseObject()
{
	bReplicates = true;

	MeshComp->SetCollisionProfileName(TEXT("BlockAll"));

	// Camera
	HideCameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("HideCamComponent"));
	HideCameraComp->SetupAttachment(RootSceneComp);

	// Effect
	HideEffectComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("HideEffectComponent"));
	HideEffectComp->SetupAttachment(RootComponent);

}

void AInteractHideObject::BeginPlay()
{
	Super::BeginPlay();

	if (HideEffectComp)
	{
		HideEffectComp->SetActive(false);
		HideEffectComp->SetVisibility(false);
		HideEffectComp->OnSystemFinished.AddDynamic(this, &AInteractHideObject::OnEffectFinished);
	}

}

void AInteractHideObject::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AInteractHideObject, bIsInHideView);
	DOREPLIFETIME(AInteractHideObject, PreviousViewTarget);
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
	UE_LOG(LogTemp, Warning, TEXT("InteractHideObject::OnInteractClient"));

	if (HasAuthority()) return;

	if (!Interactor->IsLocallyControlled()) return;

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
		SetViewTarget(InteractorPC, PreviousViewTarget);
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
		SetViewTarget(InteractorPC, PreviousViewTarget);
	}
}

void AInteractHideObject::EnterObject(APlayerController* InteractorPC)
{
	// 현재 뷰 타겟 저장
	PreviousViewTarget = InteractorPC->GetViewTarget();

	bIsInHideView = true;
	HidePlayer = InteractorPC->GetPawn();
}

void AInteractHideObject::ExitObject(APlayerController* InteractorPC)
{
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
		InteractorPC->SetViewTargetWithBlend(NewViewTarget, CameraBlendTime);

		UE_LOG(LogTemp, Log, TEXT("Client: SetViewTarget called with actor: %s"),
			*NewViewTarget->GetName());
	}
}

void AInteractHideObject::S2A_PlayEffect_Implementation(APlayerController* InteractorPC)
{
	if (HideEffectComp)
	{
		// 이펙트 활성화 및 보이게 설정
		HideEffectComp->SetActive(false);
		HideEffectComp->SetVisibility(false);
	}
}

void AInteractHideObject::OnEffectFinished(UNiagaraComponent* PSystem)
{
	if (HideEffectComp)
	{
		HideEffectComp->SetActive(false);
		HideEffectComp->SetVisibility(false);
	}
}