// GameplayTagNavLinkComponent.h

#pragma once

#include "CoreMinimal.h"
#include "NavLinkCustomComponent.h"
#include "GameplayTagContainer.h"
#include "GameplayTagNavLinkComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class NEW_THEPHANTOMTWINS_API UGameplayTagNavLinkComponent : public UNavLinkCustomComponent
{
    GENERATED_BODY()

public:
    UGameplayTagNavLinkComponent();

    virtual void BeginPlay() override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GAS")
    FGameplayTag AbilityTag;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GAS")
    TObjectPtr<AActor> TargetActor;

    //UFUNCTION()
    //void HandleSmartLinkReached(AActor* MovingActor, const FVector& DestinationPoint);
    UFUNCTION()
    void HandleSmartLinkReached(UNavLinkCustomComponent* LinkComp, UObject* PathingAgent, const FVector& DestPoint);
};
