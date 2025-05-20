// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SzComponents/Interaction.h"
#include "BaseObject.generated.h"

UENUM(BlueprintType)
enum class EObjectType : uint8
{
	Item	UMETA(DisplayName = "아이템"),
	Story	UMETA(DisplayName = "스토리"),
	Text	UMETA(DisplayName = "텍스트"),
	Cutscene UMETA(DisplayName = "연출"),
	Hidden	UMETA(DisplayName = "숨기"),
	Tool	UMETA(DisplayName = "도구")
};

UCLASS()
class OBJECT_PLUGINS_API ABaseObject : public AActor, public IInteraction
{
	GENERATED_BODY()
	
public:	
	ABaseObject();

protected:
	virtual void BeginPlay() override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

public:
	virtual void OnInteractSever_Implementation(APawn* Interactor) override;
	virtual void OnInteractClient_Implementation(APawn* Interactor) override;
	virtual bool CanInteract_Implementation(const APawn* Interactor) const override;
	virtual bool GetPickedUp_Implementation() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UStaticMeshComponent> MeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UInteractableComponent> InteractComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	bool bDestory = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
	bool bCanInteract = true;

	// 오브젝트 타입 (아이템, 텍스트, 도구 등)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	EObjectType ObjectType = EObjectType::Item;

	// 인벤토리 획득 여부 (아이템, 텍스트, 도구일 때만 노출)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction", meta = (EditCondition = "bIsInventoryObject", EditConditionHides))
	bool bIsPickedUp = false;

	// UI 사용 여부
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	bool bUseUI = false;

	// UI 위젯 컴포넌트 (UI 사용 시에만 활성화)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI", meta = (EditCondition = "bUseUI", EditConditionHides))
	TObjectPtr<class UWidgetComponent> WidgetComponent;

	// UI 클래스 설정 (UI 사용 시에만 노출)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI", meta = (EditCondition = "bUseUI", EditConditionHides))
	TSubclassOf<class UUserWidget> WidgetClass;

private:
	// 내부용: bIsPickedUp 노출 조건
	UPROPERTY()
	bool bIsInventoryObject = true;

};
