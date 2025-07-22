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

    // 블루프린트에서 동작을 구현할 수 있도록 선언(서버용)
	UFUNCTION(BlueprintNativeEvent, Category = "ItemObject")
	void UseItemEffectServer(const APawn* Interactor);
	virtual void UseItemEffectServer_Implementation(const APawn* Interactor){}

    // (Client용)
    UFUNCTION(BlueprintNativeEvent, Category = "ItemObject")
	void UseItemEffectClient(const APawn* Interactor);
	virtual void UseItemEffectClient_Implementation(const APawn* Interactor){}

    UFUNCTION(BlueprintCallable, Category = "ItemObject")
	void DestroyItem();

	virtual void OnRep_bIsActived() override;

	// 아이템을 먹었을 때 먹은 대상에게 게임플레이 이펙트를 적용시켜주는 함수
	void ApplyEffectToTarget(const APawn* Interactor);	

	// 아이템에 대한 자체 이펙트(상자 오픈 이펙트)를 재생하기 위한 함수
	void InvokeGameplayCue(const APawn* Interactor);		

protected:
	UPROPERTY()
	TObjectPtr<class UAbilitySystemComponent> ASC;

	UPROPERTY(EditAnywhere, Category = GAS)
	TSubclassOf<class UGameplayEffect> GameplayEffectClass;

	UPROPERTY(EditAnywhere, Category = GAS, Meta=(Categories=GameplayCue))
	FGameplayTag GameplayCueTag;
};

// AItemObject : 즉발이든 인벤토리 들어가든 interact하면 무조건 파괴되는 오브젝트
// 수국, 글리치함정, 회복 비콘은 InteractableObject