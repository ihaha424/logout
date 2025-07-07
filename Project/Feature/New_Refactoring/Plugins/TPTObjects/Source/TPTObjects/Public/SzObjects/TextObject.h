// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SzObjects/BaseObject.h"
#include "TextObject.generated.h"

UCLASS()
class TPTOBJECTS_API ATextObject : public ABaseObject
{
	GENERATED_BODY()
	
public:	
	ATextObject();

protected:
    UFUNCTION(BlueprintNativeEvent, Category = "TextObject")
    void SetData2Widget();	// data를 위젯으로 세팅(자식클래스든, BP에서든 무조건 구현해줘야함)
	void SetData2Widget_Implementation();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TextObject")
	TSubclassOf<class UUserWidget> AttachWidgetClass;

};
