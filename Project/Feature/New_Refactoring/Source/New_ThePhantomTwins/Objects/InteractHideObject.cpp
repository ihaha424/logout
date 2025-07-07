
#include "InteractHideObject.h"
#include "Net/UnrealNetwork.h"
#include "Camera/CameraComponent.h"


AInteractHideObject::AInteractHideObject() : ABaseObject()
{
	MeshComp->SetCollisionProfileName(TEXT("BlockAll"));

	// Camera
	HideCameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("BideCamComponent"));
	HideCameraComp->SetupAttachment(RootSceneComp);

}

void AInteractHideObject::BeginPlay()
{
	Super::BeginPlay();

}

void AInteractHideObject::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//DOREPLIFETIME(AInteractHideObject, );
}

void AInteractHideObject::OnInteractServer_Implementation(const APawn* Interactor)
{
	// 플레이어가 들어있다는거 체크
}

void AInteractHideObject::OnInteractClient_Implementation(const APawn* Interactor)
{
	// 플레이어 카메라를 object 카메라로 돌리기
}

bool AInteractHideObject::CanInteract_Implementation(const APawn* Interactor)
{
	SetWidgetVisible(bCanInteract);

	return bCanInteract;
}