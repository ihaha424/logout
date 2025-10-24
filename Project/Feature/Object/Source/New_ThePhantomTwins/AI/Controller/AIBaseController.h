// AIBaseController.h

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "AbilitySystemInterface.h"
#include "AIBaseController.generated.h"

class UAIPerceptionComponent;
class UAISenseConfig_Sight;
class UAISenseConfig_Hearing;

UCLASS()
class NEW_THEPHANTOMTWINS_API AAIBaseController : public AAIController, public IAbilitySystemInterface
{
    GENERATED_BODY()

public:
    AAIBaseController();

    //~ Begin AI Perceptions
    void AddPerceptionSightList(AActor* Actor);
    void RemovePerceptionSightList(AActor* Actor);
    void ResetSightList();
    //~ End AI Perceptions

protected:
    //~ Begin AIController
    virtual void OnPossess(APawn* InPawn) override;
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    virtual void Tick(float DeltaTime) override;
    //~ End AIController

    //~ Begin AI Perceptions
    UPROPERTY()
    TObjectPtr<UAISenseConfig_Sight> SightConfig;
    UPROPERTY()
    TObjectPtr<UAISenseConfig_Hearing> HearingConfig;
    UFUNCTION()
    void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
    FTimerHandle SightTimerHandle;
    UPROPERTY()
    TArray<TWeakObjectPtr<AActor>> PerceptionSightList;
    void FindCloseActor();
    UPROPERTY()
    TMap<FName, FUintVector2> StimulusPriorityMap;
    inline int32 GetStimulusPriority(const FName& Tag) const;
    inline int32 GetStimulusStrength(const FName& Tag) const;
    virtual bool CheckTargetActorType(UAbilitySystemComponent* ASC) const;
    //~ End AI Perceptions

    //~ Begin BeHaviorTree
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    TObjectPtr<UBehaviorTree> BehaviorTree;
    //~ End BeHaviorTree

    //~ Begin IAbilitySystemInterface interface & Additional GAS System
    virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
    //~ End IAbilitySystemInterface interface & Additional GAS System

private:
    inline void SetPerceptionByCharacterAttributeSet(APawn* InPawn);
};
