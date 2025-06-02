// Fill out your copyright notice in the Description page of Project Settings.


#include "SzObjects/CCTV.h"
#include "GameFramework/PlayerController.h" // 플레이어 컨트롤러 헤더 추가
#include "Kismet/GameplayStatics.h" // 게임플레이 스태틱스 헤더 추가
#include "Camera/CameraComponent.h" // 카메라 컴포넌트 헤더 추가

ACCTV::ACCTV()
{
	PrimaryActorTick.bCanEverTick = false;

    CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
    CameraComp->SetupAttachment(RootComponent);

}

void ACCTV::BeginPlay()
{
	Super::BeginPlay();
}

void ACCTV::OnInteractSever_Implementation(APawn* Interactor)
{
    UE_LOG(LogTemp, Warning, TEXT("ACCTV::OnInteract"));

    // 플레이어 컨트롤러 가져오기
    APlayerController* PlayerController = Cast<APlayerController>(Interactor->GetController());// UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (!PlayerController)
    {
        UE_LOG(LogTemp, Error, TEXT("PlayerController is null in HideComponent::Execute"));
        return;
    }

	// CCTV 카메라로 전환
    // 카메라 컴포넌트 찾기 (처음 실행 시)
    if (!CameraComp)
    {
        UE_LOG(LogTemp, Error, TEXT("No CameraComponent"));
        return;
    }

    // 카메라 전환 로직
    if (!bIsInCCTVView)
    {
        // 전환 전 입력 비활성화
        PlayerController->SetIgnoreMoveInput(true);
        PlayerController->SetIgnoreLookInput(true);

        // 현재 뷰 타겟 저장
        PreviousViewTarget = PlayerController->GetViewTarget();

        // 오너 액터(BP_HideCamChange)의 카메라로 전환
        PlayerController->SetViewTarget(this);

        // 상태 변경
        bIsInCCTVView = true;
    }
    else
    {
        // 이전 뷰 타겟(BP_ThirdPersonCharacter)으로 돌아가기
        if (PreviousViewTarget)
        {
            PlayerController->SetViewTarget(PreviousViewTarget);
        }
        else
        {
            // 이전 뷰 타겟이 없으면 플레이어의 Pawn을 사용
            PlayerController->SetViewTarget(Interactor);
        }

        // 입력 다시 활성화
        PlayerController->SetIgnoreMoveInput(false);
        PlayerController->SetIgnoreLookInput(false);

        // 상태 변경
        bIsInCCTVView = false;
    }
}

bool ACCTV::CanInteract_Implementation(const APawn* Interactor) const
{
	// 플레이어가 등급에 맞는 키를 갖고 있으면 true
	return bHasKey;
}
