// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DialogManager/DialogNode.h"
#include "DialogManager/DialogDatabaseAsset.h"
#include "DialogManager.generated.h"

template <typename T>
concept EnumValue = std::is_enum_v<T>;

UCLASS(BlueprintType, EditInlineNew, DefaultToInstanced)
class NEW_THEPHANTOMTWINS_API UDialogManager : public UObject
{
	GENERATED_BODY()

public:
	void Initialize(TMap<TSubclassOf<UUserWidget>, TObjectPtr<UDataTable>>& InitializeData);

	UFUNCTION(BlueprintCallable, Category = "DialogManager | DialogNode")
	int32 NextSequence(int32 Jump = 1);

	UFUNCTION(BlueprintCallable, Category = "DialogManager | DialogNode")
	void SetSequence(const TArray<int32>& LevelIndex);
	template<EnumValue Enum>
	void SetSequence(const TArray<Enum>& LevelIndex)
	{
		TArray<int32> AsInt;
		AsInt.Reserve(LevelIndex.Num());
		for (Enum E : LevelIndex)
		{
			using U = std::underlying_type_t<Enum>;
			AsInt.Add(static_cast<int32>(static_cast<U>(E)));
		}
		SetSequence(AsInt);
	}

	UFUNCTION(BlueprintCallable, Category = "DialogManager | DialogNode")
	TArray<int32> GetSequence() const;
	template<EnumValue Enum>
	TArray<Enum> GetSequence() const
	{
		TArray<int32> CurrentSequence = GetSequence();
		TArray<Enum> Out;
		Out.Reserve(CurrentSequence.Num());
		for (int32 V : CurrentSequence)
		{
			Out.Add(static_cast<Enum>(V));
		}
		return Out;
	}

	UFUNCTION(BlueprintCallable, Category = "DialogManager | DialogNode")
	int32 EventTrriger(int32 Level, int32 index) const;
	template<EnumValue Enum>
	Enum EventTrriger(Enum Level, Enum Index) const
	{
		using U = std::underlying_type_t<Enum>;
		const int32 L = static_cast<int32>(static_cast<U>(Level));
		const int32 I = static_cast<int32>(static_cast<U>(Index));
		int32 out = EventTrriger(L, I);
		return static_cast<Enum>(out);
	}

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DialogManager | Database")
	TObjectPtr<UDialogDatabaseAsset> DataBase;

	FDialogNode DialogNode;
};
