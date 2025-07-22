// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SzObjects/InteractableObject.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "ItemObject.generated.h"

UCLASS()
class TPTOBJECTS_API AItemObject : public AInteractableObject, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
	AItemObject();

protected:
    virtual void BeginPlay() override;

public:
	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents() override;

    virtual void OnInteractServer_Implementation(const APawn* Interactor) override;
    virtual void OnInteractClient_Implementation(const APawn* Interactor) override;

    // 자식클래스 혹은 블루프린트에서 동작을 구현할 수 있도록 선언(서버용)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ItemObject")
	void UseItemEffectServer(const APawn* Interactor);
	virtual void UseItemEffectServer_Implementation(const APawn* Interactor);

    // (Client용)
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ItemObject")
	void UseItemEffectClient(const APawn* Interactor);
	virtual void UseItemEffectClient_Implementation(const APawn* Interactor);

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "ItemObject")
	void DestroyItem();
	virtual void DestroyItem_Implementation();


	void ApplyEffectToTarget(const APawn* Interactor);	// 아이템을 먹었을 때 먹은 대상에게 게임플레이 이펙트를 적용시켜주는 함수
	void InvokeGameplayCue(const APawn* Interactor);		// 아이템에 대한 자체 이펙트(상자 오픈 이펙트)를 재생하기 위한 함수

protected:
	UPROPERTY()
	TObjectPtr<class UAbilitySystemComponent> ASC;

	UPROPERTY(EditAnywhere, Category = GAS)
	TSubclassOf<class UGameplayEffect> GameplayEffectClass;

	UPROPERTY(EditAnywhere, Category = GAS, Meta=(Categories=GameplayCue))
	FGameplayTag GameplayCueTag;
};
