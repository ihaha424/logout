// AIBaseController.h

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "AIBaseController.generated.h"

class UAIPerceptionComponent;
class UAISenseConfig_Sight;
class UAISenseConfig_Hearing;

UCLASS()
class NEW_THEPHANTOMTWINS_API AAIBaseController : public AAIController
{
    GENERATED_BODY()

public:
    AAIBaseController();

protected:
    virtual void OnPossess(APawn* InPawn) override;
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    //~ Begin AI Perceptions
    UPROPERTY()
    TObjectPtr<UAISenseConfig_Sight> SightConfig;
    UPROPERTY()
    TObjectPtr<UAISenseConfig_Hearing> HearingConfig;
    UFUNCTION()
    void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
    //~ Begin AI Perceptions

    //~ Begin BeHaviorTree
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    TObjectPtr<UBehaviorTree> BehaviorTree;
    //~ Begin BeHaviorTree


private:
    inline void SetPerceptionByCharacterAttributeSet(APawn* InPawn);
};
