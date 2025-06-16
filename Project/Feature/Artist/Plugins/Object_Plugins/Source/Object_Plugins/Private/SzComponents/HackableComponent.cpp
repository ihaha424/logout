// Fill out your copyright notice in the Description page of Project Settings.


#include "SzComponents/HackableComponent.h"
#include "Net/UnrealNetwork.h"
#include "Blueprint/UserWidget.h"
#include "SzUI/HackingGauge.h"
#include "Engine/Engine.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"

// Sets default values for this component's properties
UHackableComponent::UHackableComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true); // 컴포넌트 복제 활성화

	bIsHacking = false;
	bIsHacked = false;
	HackingStartTime = 0.0f;
	GuageUI = nullptr;
	bAutoHackingCompleted = false;
	CurrentHackingPlayer = nullptr;
}

void UHackableComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UHackableComponent, HackingStartTime);
	DOREPLIFETIME(UHackableComponent, bIsHacking);
	DOREPLIFETIME(UHackableComponent, bIsHacked);
	DOREPLIFETIME(UHackableComponent, bKeepHacked);
	DOREPLIFETIME(UHackableComponent, CurrentHackingPlayer);
}

void UHackableComponent::HackingStarted(APawn* Interactor)
{
	// 해킹 되어있거나 해킹 중이면 return
	if (bIsHacked || bIsHacking) return;

	APlayerController* InstigatorController = GetPlayerControllerFromPawn(Interactor);
	ensureMsgf(InstigatorController, TEXT("InstigatorController is invalid"));

	UE_LOG(LogTemp, Log, TEXT("Hacking : Started"));

	// 현재 시간을 저장 (시작 시간) :: 상태 업데이트 (모든 클라이언트에 복제됨)
	bIsHacking = true;
	bAutoHackingCompleted = false;
	CurrentHackingPlayer = InstigatorController;
	HackingStartTime = GetWorld()->GetTimeSeconds();

	// 해킹을 시작한 플레이어에게만 UI 표시
	if (InstigatorController)
	{
		S2C_ShowHackingUI();
	}
}

void UHackableComponent::HackingCompleted(APawn* Interactor)
{
	if (!bIsHacking || bAutoHackingCompleted) return;

	APlayerController* InstigatorController = GetPlayerControllerFromPawn(Interactor);
	ensureMsgf(InstigatorController, TEXT("InstigatorController is invalid"));
	
	if (CurrentHackingPlayer != InstigatorController) return; // 해킹을 시작한 플레이어만 완료 가능

	const float CurrentTime = GetWorld()->GetTimeSeconds();
	const float HeldDuration = CurrentTime - HackingStartTime;

	if (HeldDuration >= RequiredTime)
	{
		return;	// 이미 성공 처리된 경우는 무시 (이중 처리 방지)
	}
	else
	{
		// 실패 처리
		UE_LOG(LogTemp, Warning, TEXT("Hacking Failed: %.2f / %.2f"), HeldDuration, RequiredTime);
		CheckHackReset(Interactor);
	}
}

void UHackableComponent::UpdateHackingProgress(APawn* Interactor, float CurrentTime)
{
	APlayerController* InstigatorController = GetPlayerControllerFromPawn(Interactor);
	ensureMsgf(InstigatorController, TEXT("InstigatorController is invalid"));

	if (!bIsHacking || bIsHacked) return;
	if (CurrentHackingPlayer != InstigatorController) return; // 해킹을 시작한 플레이어만 업데이트 가능

	float HeldDuration = CurrentTime - HackingStartTime;

	// 해킹을 시작한 플레이어에게만 해킹중 UI 시간 업데이트 전송
	S2C_UpdateHackingProgress(HeldDuration);

	// HeldTime이 충분하면 자동으로 해킹 성공 처리
	if (HeldDuration >= RequiredTime)
	{
		TryCompleteHacking(Interactor, HeldDuration, CurrentTime);
		bAutoHackingCompleted = true;
	}
}

void UHackableComponent::TryCompleteHacking(APawn* Interactor, float HeldDuration, float CurrentTime)
{
	if (bIsHacked) return;

	APlayerController* InstigatorController = GetPlayerControllerFromPawn(Interactor);
	ensureMsgf(InstigatorController, TEXT("InstigatorController is invalid"));
	
	if (CurrentHackingPlayer != InstigatorController) return;

	UE_LOG(LogTemp, Log, TEXT("Hacking Success! %.2f / %.2f"), HeldDuration, RequiredTime);

	// 상태 업데이트 (모든 클라이언트에 복제됨)
	bIsHacking = false;
	bIsHacked = true;
	HackingStartTime = CurrentTime;

	// 해킹을 완료한 플레이어에게만 완료 메시지 표시
	S2C_ShowCompletedMessage();
}

void UHackableComponent::CheckHackReset()
{
	// 상태 초기화 (모든 클라이언트에 복제됨)
	bIsHacking = false;
	bIsHacked = false;
	HackingStartTime = 0.0f;
	bAutoHackingCompleted = false;


	//S2C_HideHackingUI();


	// CurrentHackingPlayer 초기화
	CurrentHackingPlayer = nullptr;

	UE_LOG(LogTemp, Log, TEXT("해킹 상태 초기화 완료"));
}

void UHackableComponent::CheckHackReset(APawn* Interactor)
{
	// 상태 초기화 (모든 클라이언트에 복제됨)
	bIsHacking = false;
	bIsHacked = false;
	HackingStartTime = 0.0f;
	bAutoHackingCompleted = false;

	// 해킹을 시작했던 플레이어에게만 UI 숨김
		// CurrentHackingPlayer가 이 Component의 Owner와 같다면 UI 숨김

	if (CurrentHackingPlayer == GetPlayerControllerFromPawn(Interactor))
	{
		S2C_HideHackingUI();
	}

	// CurrentHackingPlayer 초기화
	CurrentHackingPlayer = nullptr;

	UE_LOG(LogTemp, Log, TEXT("해킹 상태 초기화 완료"));
}

APlayerController* UHackableComponent::GetPlayerControllerFromPawn(APawn* Pawn)
{
	if (!Pawn) return nullptr;

	return Cast<APlayerController>(Pawn->GetController());
}

void UHackableComponent::S2C_ShowHackingUI_Implementation()
{
	// 로컬 플레이어 컨트롤러에서만 실행됨
	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		if (PC->IsLocalPlayerController())
		{
			// 이미 UI가 있으면 제거
			if (GuageUI)
			{
				GuageUI->RemoveFromParent();
				GuageUI = nullptr;
			}

			// 새 UI 위젯 생성 및 화면에 추가
			if (HackingGaugeWidget)
			{
				GuageUI = CreateWidget<UUserWidget>(PC, HackingGaugeWidget);
				if (GuageUI)
				{
					GuageUI->AddToViewport();
				}
			}
		}
	}
}

void UHackableComponent::S2C_UpdateHackingProgress_Implementation(float HeldDuration)
{
	// 해킹중 UI 시간 업데이트 (로컬에서만)
	if (GuageUI)
	{
		if (UHackingGauge* Widget = Cast<UHackingGauge>(GuageUI))
		{
			Widget->UpdateHoldTime(HeldDuration);
		}
	}
}

void UHackableComponent::S2C_ShowCompletedMessage_Implementation()
{
	if (GuageUI)
	{
		if (UHackingGauge* Widget = Cast<UHackingGauge>(GuageUI))
		{
			Widget->ShowCompletedMessage();
		}
	}
}

void UHackableComponent::S2C_HideHackingUI_Implementation()
{
	if (GuageUI)
	{
		GuageUI->RemoveFromParent();
		GuageUI = nullptr;
	}
}
