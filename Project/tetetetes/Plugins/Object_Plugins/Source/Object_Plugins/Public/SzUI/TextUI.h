// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FUIMessageRow.h"
#include "TextUI.generated.h"

/**
 * 
 */
UCLASS()
class OBJECT_PLUGINS_API UTextUI : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UTextUI(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct() override;

protected:
// 텍스트 블록 바인딩 (디자이너에서 이름 일치)
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<class UTextBlock> TextTextN;
    
    // 데이터 테이블을 에디터에서 할당
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data", meta = (ToolTip = "UI 메시지 데이터 테이블"))
    TObjectPtr<class UDataTable> UITextDataTable;
    
    // 찾을 Row 이름 (기본값 "B")
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data", meta = (ToolTip = "데이터 테이블에서 검색할 Row 이름"))
    FName RowName;
};
