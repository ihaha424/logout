// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DialogManager/DialogNode.h"
#include "DialogManager/DialogDatabaseAsset.h"
#include "DialogManager.generated.h"

template <typename T>
concept EnumValue = std::is_enum_v<T>;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FExcuteByDialogEvent, int32, EventIndex);

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
	int32 EventTrriger(int32 Level, int32 Index) const;
	template<EnumValue Enum>
	Enum EventTrriger(Enum Level, Enum Index) const
	{
		using U = std::underlying_type_t<Enum>;
		const int32 L = static_cast<int32>(static_cast<U>(Level));
		const int32 I = static_cast<int32>(static_cast<U>(Index));
		int32 out = EventTrriger(L, I);
		return static_cast<Enum>(out);
	}
	UFUNCTION(BlueprintCallable, Category = "DialogManager | EvnetDelegate")
	bool AddByDialogEvent(int32 Index, UObject* Target, FName FunctionName);
	template<class UserClass>
	bool AddByDialogEvent(int32 Index, UserClass* Listener, void (UserClass::* Func)(int32))
	{
		if (!Listener || !Func) return false;
		auto& Ev = ExcuteByDialogEventMap.FindOrAdd(Index);
		Ev.AddDynamic(Listener, Func);
		return true;
	}

	UFUNCTION(BlueprintCallable, Category = "DialogManager | EvnetDelegate")
	bool RemoveyDialogEvent(int32 Index, UObject* Target, FName FunctionName);
	template<class UserClass>
	bool RemoveByDialogEvent(int32 Index, UserClass* Listener, void (UserClass::* Func)(int32))
	{
		if (FExcuteByDialogEvent* Ev = ExcuteByDialogEventMap.Find(Index))
		{
			Ev->RemoveDynamic(Listener, Func);
			return true;
		}
		return false;
	}

	UFUNCTION(BlueprintCallable, Category = "DialogManager | EvnetDelegate")
	bool ExcuteByDialogEvent(int32 Index);


protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DialogManager | Database")
	TObjectPtr<UDialogDatabaseAsset> DataBase;

	UPROPERTY()
	TMap<int32, FExcuteByDialogEvent> ExcuteByDialogEventMap;

	FDialogNode DialogNode;
};
