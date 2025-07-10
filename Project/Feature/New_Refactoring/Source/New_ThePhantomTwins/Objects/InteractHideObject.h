#pragma once

#include "CoreMinimal.h"
#include "SzObjects/StaticObject.h"
#include "SzInterface/Interact.h"
#include "SzObjects/ObjectState.h"
#include "InteractHideObject.generated.h"

/**
 * 
 */
UCLASS()
class NEW_THEPHANTOMTWINS_API AInteractHideObject : public AStaticObject, public IInteract
{
	GENERATED_BODY()

public:
	AInteractHideObject();


protected:
	virtual void BeginPlay() override;

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual bool CanInteract_Implementation(const APawn* Interactor, bool bIsDetected) override;
	virtual void OnInteractServer_Implementation(const APawn* Interactor) override;
	virtual void OnInteractClient_Implementation(const APawn* Interactor) override;

protected:
	void CamLogicServer(APlayerController* InteractorPC);
	void CamLogicClient(APlayerController* InteractorPC);

	void EnterObject(APlayerController* InteractorPC);
	void ExitObject(APlayerController* InteractorPC);

	void SetInputState(APlayerController* InteractorPC, bool bIgnoreInput);		// 입력 제어를 위한 함수
	void SetViewTarget(APlayerController* InteractorPC, AActor* NewViewTarget); // 카메라 전환을 위한 함수


	UFUNCTION(NetMulticast, Reliable)
	void S2A_PlayEffect(APlayerController* InteractorPC);
	void S2A_PlayEffect_Implementation(APlayerController* InteractorPC);

	void OnEffectFinished();

public:
	// 카메라
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "HideObject | Camera")
	TObjectPtr<class UCameraComponent> HideCameraComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="HideObject | Effects")
	TObjectPtr<class UNiagaraComponent> HideEffectComp;

	// 블렌드 시간 설정
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HideObject | Camera")
	float CameraBlendTime = 0.75f;

	// 카메라 전환을 위한 추가 변수
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HideObject | Camera", Replicated)
	bool bIsInHideView = false;

	// 이전 뷰 타겟 저장 (임시)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HideObject | Camera", Replicated)
	TObjectPtr <AActor> PreviousViewTarget = nullptr;

	// 숨는 Player
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HideObject", Replicated)
	TObjectPtr <AActor> HidePlayer = nullptr;


};
