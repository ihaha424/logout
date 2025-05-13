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
	virtual void OnInteract_Implementation(APawn* Interactor) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UStaticMeshComponent> MeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UInteractableComponent> InteractComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	bool bDestory = false;

	// 오브젝트 타입 (아이템, 텍스트, 도구 등)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	EObjectType ObjectType = EObjectType::Item;

	// 인벤토리 획득 여부 (아이템, 텍스트, 도구일 때만 노출)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction", meta = (EditCondition = "bIsInventoryObject", EditConditionHides))
	bool bIsPickedUp = false;

private:
	// 내부용: bIsPickedUp 노출 조건
	UPROPERTY()
	bool bIsInventoryObject = true;

};
