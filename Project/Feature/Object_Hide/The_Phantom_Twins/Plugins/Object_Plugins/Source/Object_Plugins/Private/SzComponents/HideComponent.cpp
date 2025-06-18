// Fill out your copyright notice in the Description page of Project Settings.


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
    APlayerController* PlayerController = Cast<APlayerController>(Interactor->GetController());// UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (!PlayerController)
    {
        UE_LOG(LogTemp, Error, TEXT("PlayerController is null in HideComponent::Execute"));
        return;
    }

    if (nullptr != HidePlayer && HidePlayer != PlayerController)
        return;


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
    
    if (bUseCamera && HideCamera)
    {
        // 카메라 전환 로직
        if (!bIsInHideView)
        {
            // 전환 전 입력 비활성화
            PlayerController->SetIgnoreMoveInput(true);
            PlayerController->SetIgnoreLookInput(true);

            // 현재 뷰 타겟 저장
            PreviousViewTarget = PlayerController->GetViewTarget();

            // 오너 액터(BP_HideCamChange)의 카메라로 전환
            PlayerController->SetViewTargetWithBlend(OwnerActor, CameraBlendTime);

            // 상태 변경
            bIsInHideView = true;
            bHasPlayer = true;
            HidePlayer = PlayerController;

            Debug_bHasPlayer();

        }
        else
        {
            // 이전 뷰 타겟(BP_ThirdPersonCharacter)으로 돌아가기
            if (PreviousViewTarget)
            {
                PlayerController->SetViewTargetWithBlend(PreviousViewTarget, CameraBlendTime);
            }
            else
            {
                // 이전 뷰 타겟이 없으면 플레이어의 Pawn을 사용
                PlayerController->SetViewTargetWithBlend(Interactor, CameraBlendTime);
            }

            // 입력 다시 활성화
            PlayerController->SetIgnoreMoveInput(false);
            PlayerController->SetIgnoreLookInput(false);

            // 상태 변경
            bIsInHideView = false;
            bHasPlayer = false;
            HidePlayer = nullptr;

            Debug_bHasPlayer();
        }
    }
    else
    {
        // 카메라 없는 경우 기본 숨기 동작
        UE_LOG(LogTemp, Log, TEXT("Performing basic hide action"));
        // 여기에 추가적인 숨기 로직 작성 (예: 캐릭터 비활성화)
    }

}

void UHideComponent::OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APawn* Pawn = Cast<APawn>(OtherActor);
	if (Pawn && !bUseCamera)
	{
		bHasPlayer = true;
	}

    Debug_bHasPlayer();

}

void UHideComponent::OnTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	APawn* Pawn = Cast<APawn>(OtherActor);
	if (Pawn && !bUseCamera)
	{
		bHasPlayer = false;
	}

    Debug_bHasPlayer();
}

void UHideComponent::Debug_bHasPlayer()
{
    // 로그 출력
    if (GEngine)
    {
        FString RoleString = GetOwner()->HasAuthority() ? TEXT("Server") : TEXT("Client");
        ENetMode NetMode = GetNetMode();
        FString NetModeString;
        switch (NetMode)
        {
        case NM_Standalone: NetModeString = TEXT("Standalone"); break;
        case NM_ListenServer: NetModeString = TEXT("ListenServer"); break;
        case NM_Client: NetModeString = TEXT("Client"); break;
        default: NetModeString = TEXT("Unknown"); break;
        }
        GEngine->AddOnScreenDebugMessage(
            -1, 5.f, FColor::Yellow,
            FString::Printf(TEXT("[EndOverlap] %s (%s): bHasPlayer = %s"),
                *RoleString, *NetModeString, bHasPlayer ? TEXT("True") : TEXT("False"))
        );
    }
}
