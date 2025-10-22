#pragma once

#include "CoreMinimal.h"
#include "SzObjects/InteractableObject.h"
#include "SzInterface/Destroyable.h"
#include "InteractHideObject.generated.h"

class UGameplayEffect;

UCLASS()
class NEW_THEPHANTOMTWINS_API AInteractHideObject : public AInteractableObject, public IDestroyable
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

	virtual void OnDestroy_Implementation(const APawn* Interactor) override;

protected:
	void CamLogicServer(const APawn* Interactor);
	void CamLogicClient(const APawn* Interactor);

	void EnterObject(const APawn* Interactor);
	void ExitObject();

	UFUNCTION(BlueprintCallable, Category = "HideObject")
	void SetInputState(APlayerController* InteractorPC, bool bIgnoreInput);		// 입력 제어를 위한 함수


	UFUNCTION(BlueprintCallable, Category = "HideObject")
	void SetViewTarget(APlayerController* InteractorPC, AActor* NewViewTarget); // 카메라 전환을 위한 함수

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void PlayHideUnable(const APawn* Interactor);

	UFUNCTION(NetMulticast, Reliable)
	void S2A_PlayEffect(FVector EffectLocation);
	void S2A_PlayEffect_Implementation(FVector EffectLocation);

	// 플레이어가 상호작용할때 S2A로 호출되는 로직(ex.문 여는 애니메이션/이펙트 발생 등). 블프에서 구현 해주세요.
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "HideObject")
	void PlayEffectLogic(FVector EffectLocation);
	void PlayEffectLogic_Implementation(FVector EffectLocation);

	void EnableVignetteEffect(bool bEnable);

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HideObject | Effects")
	TObjectPtr<class UMaterialInterface> VignetteMaterial;

	// 숨는 Player
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "HideObject", Replicated)
	TObjectPtr <APawn> HidePlayer = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HideObject | Effects")
	TSubclassOf<UGameplayEffect> HideTagGE;
};
