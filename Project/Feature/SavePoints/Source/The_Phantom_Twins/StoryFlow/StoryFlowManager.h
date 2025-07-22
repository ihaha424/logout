#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "UObject/NoExportTypes.h"
#include "Kismet/GameplayStatics.h"
#include "StoryFlowManager.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogStoryFlow, Log, All);

DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnDataChangedBP, FName, DataName, UObject*, DataObject);

UCLASS(BlueprintType)
class THE_PHANTOM_TWINS_API UStoryFlowManager : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    using FCallback = TFunction<void(FName, const UObject*)>;
    using FCallbackID = FGuid;

    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    UFUNCTION()
    void OnPreLoadMap(const FString& MapName);

    /**
     * @brief   : Blueprint-accessible functions
     * @param DataName 
     * @param Data 
     */
    UFUNCTION(BlueprintCallable, Category = "StoryFlow")
    void RegisterData(FName DataName, UObject* Data);

    UFUNCTION(BlueprintCallable, Category = "StoryFlow")
    void UnregisterData(FName DataName);

    UFUNCTION(BlueprintCallable, Category = "StoryFlow")
    void SetData(FName DataName, UObject* NewValue);

    UFUNCTION(BlueprintCallable, Category = "StoryFlow")
    UObject* GetData(FName DataName) const;

    /**
     * @brief : Subscribe from Blueprint
     */
    UFUNCTION(BlueprintCallable, Category = "StoryFlow")
    FGuid SubscribeBP(FName DataName, const FOnDataChangedBP& Callback);

    /**
     * @brief : Subscribe from C++
     */
    FGuid Subscribe(FName DataName, FCallback Callback);

    UFUNCTION(BlueprintCallable, Category = "StoryFlow")
    void Unsubscribe(FName DataName, FGuid CallbackId);

    template<typename T>
    T* GetDataAs(FName DataName) const
    {
        UObject* Obj = GetData(DataName);
        return Cast<T>(Obj);
    }

    UFUNCTION(BlueprintCallable, Category = "StoryFlow")
    void ClearAllData(bool bRemoveCoreData = false);

    UFUNCTION(BlueprintCallable, Category = "StoryFlow")
    void MarkAsCoreData(FName DataName, bool bCoreData = true);

    /**
     * @brief : Save/Load
     */

    UFUNCTION(BlueprintCallable, Category = "StoryFlow")
    void SaveToDisk();

    UFUNCTION(BlueprintCallable, Category = "StoryFlow")
    void LoadFromDisk();

    UFUNCTION(BlueprintCallable, Category = "StoryFlow")
    void SaveGameData(const FString& SlotName);

    UFUNCTION(BlueprintCallable, Category = "StoryFlow")
    void LoadGameData(const FString& SlotName);



private:
    TMap<FName, TObjectPtr<UObject>> DataMap;
    TMap<FName, TArray<TPair<FGuid, FCallback>>> Subscribers;
    TMap<FGuid, FOnDataChangedBP> BPDelegateMap;
    TSet<FGuid> DeferredRemovals;
    TSet<FName> CoreDataSet;

    void NotifySubscribers(FName DataName);
};

