#pragma once

#include "CoreMinimal.h"
#include "SzObjects/InteractableObject.h"
#include "SzInterface/Interact.h"
#include "SzObjects/ObjectState.h"
#include "InteractHideObject.generated.h"

/**
 * 
 */
UCLASS()
class NEW_THEPHANTOMTWINS_API AInteractHideObject : public AInteractableObject
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
	void CamLogicServer(const APawn* Interactor);
	void CamLogicClient(const APawn* Interactor);

	void EnterObject(const APawn* Interactor);
	void ExitObject();

	void SetInputState(APlayerController* InteractorPC, bool bIgnoreInput);		// 입력 제어를 위한 함수
	void SetViewTarget(APlayerController* InteractorPC, AActor* NewViewTarget); // 카메라 전환을 위한 함수


	UFUNCTION(NetMulticast, Reliable)
	void S2A_PlayEffect(const APawn* Interactor);
	void S2A_PlayEffect_Implementation(const APawn* Interactor);

	// 플레이어가 상호작용할때 S2A로 호출되는 로직(ex.문 여는 애니메이션/이펙트 발생 등). 블프에서 구현 해주세요.
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "HideObject")
	void PlayEffectLogic(const APawn* Interactor);
	void PlayEffectLogic_Implementation(const APawn* Interactor);



	void OnEffectFinished();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HideObject")
	TObjectPtr<class UBoxComponent> InPosBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HideObject")
	TObjectPtr<class UBoxComponent> OutPosBox;

	// AI percrption(적이 사용 -> Hide랑 문에만 추가)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HideObject | AI")
	TObjectPtr<class UAIPerceptionStimuliSourceComponent> StimuliSource;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HideObject | AI")
	TObjectPtr<class USphereComponent> SphereCollisionComp;

	// 카메라
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "HideObject | Camera")
	TObjectPtr<class UCameraComponent> HideCameraComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="HideObject | Effects")
	TObjectPtr<class UNiagaraComponent> HideEffectComp;

	// 숨는 Player
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HideObject", Replicated)
	TObjectPtr <APawn> HidePlayer = nullptr;

};
