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

protected:
	virtual void BeginPlay() override;

public:
	virtual void OnInteractServer_Implementation(const APawn* Interactor) override;

	// 모든 클라이언트에게 복제되는 함수들 추가
	UFUNCTION(NetMulticast, Reliable)
	void MulticastApplyEffect(const APawn* Interactor);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastInvokeGameplayCue(const APawn* Interactor);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastExecuteTrapBox(const APawn* Interactor);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastShowWarning();

	// 기존 함수들 (서버에서만 실행)
	void ApplyEffectToTarget(const APawn* Interactor);
	void InvokeGameplayCue(const APawn* Interactor);
	void ExecuteTrapBoxGA(const APawn* Interactor);

protected:
	UPROPERTY(EditAnywhere, Category = "BoxObject | GAS")
	TSubclassOf<class UGameplayEffect> GameplayEffectClass;

	UPROPERTY(EditAnywhere, Category = "BoxObject | GAS", Meta = (Categories = GameplayCue))
	FGameplayTag GameplayCueTag;

	UPROPERTY(EditAnywhere, Category = "BoxObject")
	bool bisTrapBox;

	UPROPERTY(EditAnywhere, Category = "BoxObject | UI")
	TSubclassOf<AActor> WarningClass;

	AActor* FindWarningActor();
};
