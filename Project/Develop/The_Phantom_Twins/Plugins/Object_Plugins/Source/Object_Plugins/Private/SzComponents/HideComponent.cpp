#include "SzComponents/HideComponent.h"
#include "SzObjects/BaseObject.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Pawn.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "GameFramework/PlayerController.h" // 플레이어 컨트롤러 헤더 추가
#include "Kismet/GameplayStatics.h" // 게임플레이 스태틱스 헤더 추가
#include "Camera/CameraComponent.h" // 카메라 컴포넌트 헤더 추가
#include "Engine/World.h"
#include "Net/UnrealNetwork.h"


//#include "Kismet/KismetSystemLibrary.h"

DEFINE_LOG_CATEGORY_STATIC(LogHideComponent, Log, All);

// Sets default values for this component's properties
UHideComponent::UHideComponent()
{
    PrimaryComponentTick.bCanEverTick = false;

    TriggerComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxTriggerComponent"));
    TriggerComponent->SetupAttachment(this);
    TriggerComponent->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
    TriggerComponent->SetGenerateOverlapEvents(true);

    SetIsReplicatedByDefault(true); // 컴포넌트 복제 활성화
}

void UHideComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(UHideComponent, bHasPlayer);
    DOREPLIFETIME(UHideComponent, bUseCamera);
    DOREPLIFETIME(UHideComponent, HidePlayer);
    DOREPLIFETIME(UHideComponent, bIsInHideView);
    DOREPLIFETIME(UHideComponent, PreviousViewTarget);
}

// Called when the game starts
void UHideComponent::BeginPlay()
{
    Super::BeginPlay();

    if (!TriggerComponent)
    {
        UE_LOG(LogTemp, Warning, TEXT("TriggerComponent is null in HideComponent"));
        return;
    }

    AActor* Owner = GetOwner();

    if (Owner)
    {
        UStaticMeshComponent* MeshComp = Owner->FindComponentByClass<UStaticMeshComponent>();
        if (MeshComp && MeshComp->GetStaticMesh()) {
            FVector Extent = MeshComp->GetStaticMesh()->GetBounds().BoxExtent;
            TriggerComponent->SetBoxExtent(Extent);
        }
    }

    TriggerComponent->OnComponentBeginOverlap.AddDynamic(this, &UHideComponent::OnTriggerBeginOverlap);
    TriggerComponent->OnComponentEndOverlap.AddDynamic(this, &UHideComponent::OnTriggerEndOverlap);
}

void UHideComponent::ExecuteSever(APawn* Interactor)
{
    // 플레이어 컨트롤러 가져오기
    APlayerController* PlayerController = CastChecked<APlayerController>(Interactor->GetController());

    if (nullptr != HidePlayer && HidePlayer != PlayerController) return;

    // 오너 액터 가져오기 (BP_HideCamChange)
    AActor* OwnerActor = GetOwner();
    if (!OwnerActor)
    {
        UE_LOG(LogTemp, Error, TEXT("OwnerActor is null in HideComponent::Execute"));
        return;
    }

    // 카메라 컴포넌트 찾기 (처음 실행 시)
    if (bUseCamera && !HideCamera)
    {
        HideCamera = OwnerActor->FindComponentByClass<UCameraComponent>();
        if (!HideCamera)
        {
            UE_LOG(LogTemp, Error, TEXT("No CameraComponent found in owner actor: %s"), *OwnerActor->GetName());
            return;
        }
    }

    if (bUseCamera)
    {
        if (bHasPlayer && HidePlayer != PlayerController) return;
        UseCamLogic(PlayerController);
    }
    else
    {
        NoCamLogic(PlayerController);
    }
}

void UHideComponent::ExecuteClient(APawn* Interactor)
{
    if (GetOwner()->HasAuthority()) return;

    // 플레이어 컨트롤러 가져오기
    APlayerController* PlayerController = CastChecked<APlayerController>(Interactor->GetController());

    if (nullptr != HidePlayer && HidePlayer != PlayerController) return;

    // 오너 액터 가져오기 (BP_HideCamChange)
    AActor* OwnerActor = GetOwner();
    if (!OwnerActor)
    {
        UE_LOG(LogTemp, Error, TEXT("OwnerActor is null in HideComponent::Execute"));
        return;
    }

    // 카메라 컴포넌트 찾기 (처음 실행 시)
    if (bUseCamera && !HideCamera)
    {
        HideCamera = OwnerActor->FindComponentByClass<UCameraComponent>();
        if (!HideCamera)
        {
            UE_LOG(LogTemp, Error, TEXT("No CameraComponent found in owner actor: %s"), *OwnerActor->GetName());
            return;
        }
    }

    if (bUseCamera)
    {
        if (bHasPlayer && HidePlayer != PlayerController) return;
        ClientUseCamLogic(PlayerController);
    }
    else
    {
        NoCamLogic(PlayerController);
    }
}


void UHideComponent::NoCamLogic(APlayerController* InteractorPC)
{
    // 카메라 없는 경우 기본 숨기 동작
    UE_LOG(LogTemp, Log, TEXT("Performing basic hide action"));
    // 여기에 추가적인 숨기 로직 작성 (예: 캐릭터 비활성화)
}

void UHideComponent::UseCamLogic(APlayerController* InteractorPC)
{
    // 서버에서 실행되는 코드
    if (!GetOwner()->HasAuthority()) return;

	if (!bIsInHideView)
	{
		EnterObject(InteractorPC);

        // 클라이언트에게 입력 비활성화 명령 전달
        SetInputState(InteractorPC, true);

        // 클라이언트에게 카메라 전환 명령 전달
        SetViewTarget(InteractorPC, GetOwner());
	}
	else
	{
		ExitObject(InteractorPC);

        // 클라이언트에게 입력 활성화 명령 전달
        SetInputState(InteractorPC, false);

        // 클라이언트에게 카메라 전환 명령 전달
        SetViewTarget(InteractorPC, PreviousViewTarget);
	}
}

void UHideComponent::ClientUseCamLogic(APlayerController* InteractorPC)
{
    if (!bIsInHideView)
    {
        // 클라이언트에게 입력 비활성화 명령 전달
        SetInputState(InteractorPC, true);

        // 클라이언트에게 카메라 전환 명령 전달
        SetViewTarget(InteractorPC, GetOwner());
    }
    else
    {
        // 클라이언트에게 입력 활성화 명령 전달
        SetInputState(InteractorPC, false);

        // 클라이언트에게 카메라 전환 명령 전달
        SetViewTarget(InteractorPC, PreviousViewTarget);
    }
}

void UHideComponent::EnterObject(APlayerController* InteractorPC)
{
    // 현재 뷰 타겟 저장
    PreviousViewTarget = InteractorPC->GetViewTarget();

    // 상태 변경
    bIsInHideView = true;
    bHasPlayer = true;
    HidePlayer = InteractorPC;
}

void UHideComponent::ExitObject(APlayerController* InteractorPC)
{
    // 상태 변경
    bIsInHideView = false;
    bHasPlayer = false;
    HidePlayer = nullptr;
}

void UHideComponent::SetInputState(APlayerController* InteractorPC, bool bIgnoreInput)
{
    //UKismetSystemLibrary::PrintString(this, "S2C_SetInputState_Implementation");

    if (InteractorPC && InteractorPC->IsLocalController())
    {
        InteractorPC->SetIgnoreMoveInput(bIgnoreInput);
        InteractorPC->SetIgnoreLookInput(bIgnoreInput);

        UE_LOG(LogTemp, Log, TEXT("Client: SetIgnoreInput called with value: %s"),
            bIgnoreInput ? TEXT("True") : TEXT("False"));
    }
}

void UHideComponent::SetViewTarget(APlayerController* InteractorPC, AActor* NewViewTarget)
{
    ULocalPlayer* LocalPlayer = InteractorPC->GetLocalPlayer();
    //UE_LOG(LogHideComponent, Log, TEXT("LocalPlayer = %s"), *LocalPlayer->GetName());

	if (InteractorPC && NewViewTarget)
	{
		InteractorPC->SetViewTargetWithBlend(NewViewTarget, CameraBlendTime);

		UE_LOG(LogTemp, Log, TEXT("Client: SetViewTarget called with actor: %s"),
			*NewViewTarget->GetName());
	}
}

void UHideComponent::OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    APawn* Pawn = Cast<APawn>(OtherActor);
    if (Pawn && !bUseCamera)
    {
        bHasPlayer = true;
    }
}

void UHideComponent::OnTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    APawn* Pawn = Cast<APawn>(OtherActor);
    if (Pawn && !bUseCamera)
    {
        bHasPlayer = false;
    }
}
