// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SzObjects/BaseObject.h"
#include "SzInterface/Interaction.h"
#include "SzObjects/ObjectState.h"
#include "ItemObject.generated.h"



UCLASS()
class OBJECT_PLUGINS_API AItemObject : public ABaseObject
{
	GENERATED_BODY()
	
public:	
	AItemObject();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void BeginPlay() override;


protected:
	UFUNCTION(BlueprintNativeEvent)
	void DestroyLogic();
	// 삭제가 필요한 경우 자식클래스나 블루프린트에서 추가해야함

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ItemObject", Replicated)
	FItemStatus ItemStatus;		// 존재이유 생각
};
