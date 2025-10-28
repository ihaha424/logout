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

	// ~Begin Main Sequence
	UFUNCTION(BlueprintCallable, Category = "DialogManager | DialogNode")
	int32 NextSequence(int32 Jump = 1, bool bExcuteBindEvent = false);

	UFUNCTION(BlueprintCallable, Category = "DialogManager | DialogNode")
	void SetSequence(const TArray<int32>& LevelIndex, bool bExcuteBindEvent = false);
	template<EnumValue Enum>
	void SetSequence(const TArray<Enum>& LevelIndex, bool bExcuteBindEvent = false)
	{
		TArray<int32> AsInt;
		AsInt.Reserve(LevelIndex.Num());
		for (Enum E : LevelIndex)
		{
			using U = std::underlying_type_t<Enum>;
			AsInt.Add(static_cast<int32>(static_cast<U>(E)));
		}
		SetSequence(AsInt, bExcuteBindEvent);
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

	//		//	~Begin Dialog Binding Event
	UFUNCTION(BlueprintCallable, Category = "DialogManager | EvnetDelegate")
	bool AddByDialogEvent(const TArray<int32>& Level, int32 Index, UObject* Target, FName FunctionName);
	template<class UserClass>
	bool AddByDialogEvent(const TArray<int32>& Level, int32 Index, UserClass* Listener, void (UserClass::* Func)(int32))
	{
		if (!Listener || !Func) return false;
		uint64  LogicalID = MakeLogicalID(Level, Index, false);
		auto& Ev = ExcuteByDialogEventMap.FindOrAdd(LogicalID);
		Ev.AddDynamic(Listener, Func);
		return true;
	}

	UFUNCTION(BlueprintCallable, Category = "DialogManager | EvnetDelegate")
	bool RemoveyDialogEvent(const TArray<int32>& Level, int32 Index, UObject* Target, FName FunctionName);
	template<class UserClass>
	bool RemoveByDialogEvent(const TArray<int32>& Level, int32 Index, UserClass* Listener, void (UserClass::* Func)(int32))
	{
		uint64  LogicalID = MakeLogicalID(Level, Index, false);
		if (FExcuteByDialogEvent* Ev = ExcuteByDialogEventMap.Find(LogicalID))
		{
			Ev->RemoveDynamic(Listener, Func);
			return true;
		}
		return false;
	}

	UFUNCTION(BlueprintCallable, Category = "DialogManager | EvnetDelegate")
	bool ExcuteByDialogEvent(const TArray<int32>& Level, int32 Index);
	//		// ~End Dialog Binding Event
	// ~End Main Sequence

	// ~Begin EventTrriger
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
	// ~End EventTrriger


protected:
	// ~Begin Database
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DialogManager | Database")
	TObjectPtr<UDialogDatabaseAsset> DataBase;
	// ~End Database


	// ~Begin Main Sequence
	FDialogNode DialogNode;
	//		//	~Begin Dialog Binding Event
	UPROPERTY()
	TMap<int64, FExcuteByDialogEvent> ExcuteByDialogEventMap;
	//		// ~End Dialog Binding Event
	// ~End Main Sequence


private:
	// Utils Function
	int32 MakeLogicalID(const TArray<int32> Level, int32 ID, bool bTriggered) const;
};
