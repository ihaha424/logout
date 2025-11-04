#pragma once

#include "CoreMinimal.h"
#include "SzObjects/InteractableObject.h"
#include "GameplayTagContainer.h"
#include "BoxObject.generated.h"

UCLASS()
class NEW_THEPHANTOMTWINS_API ABoxObject : public AInteractableObject
{
	GENERATED_BODY()
	
public:
	ABoxObject();
	UFUNCTION(BlueprintNativeEvent)
	void InitBoxOpen(bool bIsActive);
	void InitBoxOpen_Implementation(bool bIsActive);

protected:
	virtual void BeginPlay() override;
public:
	virtual void SetActive(bool bIsActive) override;
	virtual void OnInteractServer_Implementation(const APawn* Interactor) override;

	virtual void OnRep_bIsActived() override;

	// 모든 클라이언트에게 복제되는 함수들 추가
	UFUNCTION(NetMulticast, Reliable)
	void S2A_ApplyEffect(const APawn* Interactor);
	void S2A_ApplyEffect_Implementation(const APawn* Interactor);

	UFUNCTION(NetMulticast, Reliable)
	void S2A_InvokeGameplayCue(const APawn* Interactor);
	void S2A_InvokeGameplayCue_Implementation(const APawn* Interactor);

	UFUNCTION(NetMulticast, Reliable)
	void S2A_ExecuteTrapBox(const APawn* Interactor);
	void S2A_ExecuteTrapBox_Implementation(const APawn* Interactor);

	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void S2A_ShowWarning();
	void S2A_ShowWarning_Implementation();

	// 기존 함수들 (서버에서만 실행)
	void ApplyEffectToTarget(const APawn* Interactor);
	void InvokeGameplayCue(const APawn* Interactor);
	void ExecuteTrapBoxGA(const APawn* Interactor);

	// 블루프린트에서 문을 여는 동작을 구현할 수 있도록 선언
    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "BoxObject")
	void PlayWarning();

protected:
	UPROPERTY(EditAnywhere, Category = "BoxObject | GAS")
	TSubclassOf<class UGameplayEffect> GameplayEffectClass;

	UPROPERTY(EditAnywhere, Category = "BoxObject | GAS", Meta = (Categories = GameplayCue))
	FGameplayTag GameplayCueTag;

	UPROPERTY(EditAnywhere, Category = "BoxObject")
	bool bisTrapBox;

	//UPROPERTY(EditAnywhere, Category = "BoxObject | UI")
	//TSubclassOf<AActor> WarningClass;

	//AActor* FindWarningActor();

	//FTimerHandle WarningTimerHandle;
};
