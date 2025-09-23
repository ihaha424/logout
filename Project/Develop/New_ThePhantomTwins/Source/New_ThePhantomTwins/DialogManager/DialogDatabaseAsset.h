// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "StructUtils/InstancedStruct.h"
#include "DialogDatabaseAsset.generated.h"

///**
// * @brief : 커스텀 데이터용 베이스 UObject (위젯별로 자식 클래스를 파생해서 씀)
// */
//UCLASS(BlueprintType)
//class UDialogPayloadBase : public UPrimaryDataAsset
//{
//    GENERATED_BODY()
//public:
//    // 공통 필드
//};
//
//UCLASS(BlueprintType)
//class UDialogPayloadBaseText : public UDialogPayloadBase
//{
//    GENERATED_BODY()
//public:
//    UPROPERTY(EditAnywhere, BlueprintReadWrite)
//    FText Text;
//};
//
/////**
//// * @brief : 개별 데이터(한 항목)
//// */
////USTRUCT(BlueprintType)
////struct FDialogData
////{
////    GENERATED_BODY()
////
////    /**
////     * @brief   : 위젯이 해석할 커스텀 데이터 (위젯별 파생 UObject 권장)
////                : EditInlineNew + Instanced로 에디터에서 인라인 생성 가능
////     */
////    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog")
////    TSoftObjectPtr<UDialogPayloadBase> Data;
////
////    /**
////     * @brief : 트리/DB 공통으로 쓸 정수형 ID
////     */
////    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog")
////    int32 ID = 0;
////
////    // Add SoundData
////};


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


USTRUCT(BlueprintType)
struct FDialogDataTableTextTwo :public FDialogDataTableBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog")
    FText First;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog")
    FText Second;
};

USTRUCT(BlueprintType)
struct FDialogDataTableText :public FDialogDataTableBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialog")
    FText First;
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
class UDialogDataWrapper : public UObject
{
    GENERATED_BODY()
public:
    FDialogDataTableBase* Data;
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