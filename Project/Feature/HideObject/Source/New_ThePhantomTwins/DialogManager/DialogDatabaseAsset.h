// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "StructUtils/InstancedStruct.h"
#include "DialogDatabaseAsset.generated.h"

USTRUCT(BlueprintType)
struct FDialogDataTableBase :public FTableRowBase
{
    GENERATED_BODY()

    /**
     * @brief : Trigger Flag
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog")
    bool bIsTrigger = 0;

    /**
     * @brief : 트리/DB 공통으로 쓸 정수형 ID
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog")
    int32 ID = 0;

    /**
     * @brief : Level
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog")
    TArray<int32> Level;
};

// 애는 이제 데베안의 데이터
USTRUCT(BlueprintType)
struct FDialogDataById
{
    GENERATED_BODY()

    /**
     * @brief : 위젯 클래스
     */
    TSubclassOf<UUserWidget> WidgetClass;

    /**
     * @brief : 데이터 클래스
     */
    UDataTable* DataTable;

    /*
     * @brief : 데이터 Table Row Key Matching
    */
    TMap<int32, FName> DataIndexKeyMap;
};

UCLASS(BlueprintType)
class UDialogDatabaseAsset : public UObject
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog")
    TMap <TSubclassOf<UUserWidget>, FDialogDataById> Database;

public:
    bool FindDialogData(TSubclassOf<UUserWidget> WidgetClass, int32 ID, FDialogDataTableBase& OutData) const;
    UFUNCTION(BlueprintCallable, Category = "Dialog")
    bool FindDialogDataInstancedStruct(TSubclassOf<UUserWidget> WidgetClass, int32 ID, FInstancedStruct& Row) const;


    bool FindDialogDataById(int32 ID, FDialogDataTableBase& OutData, TSubclassOf<UUserWidget>& OutWidgetClass) const;
    UFUNCTION(BlueprintCallable, Category = "Dialog")
    bool FindDialogDataByIdInstancedStruct(int32 ID, FInstancedStruct& Row, TSubclassOf<UUserWidget>& OutWidgetClass) const;

};

//Helper에서 Widget타입에 따른 UI->UIMnaager에 등록하고 가져와서 1개만 쓰기

// BlueprintPure -> const함수는 BlueprintPure=false 로 하지 않으면 자동으로 pure함수가 된다.
// pure함수는 데이터가 참조 될때 마다 실행이 된다.