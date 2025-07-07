#pragma once

#include "CoreMinimal.h"
#include "SzObjects/BaseObject.h"
#include "SzInterface/Interact.h"
#include "SzObjects/ObjectState.h"
#include "InteractHideObject.generated.h"

/**
 * 
 */
UCLASS()
class NEW_THEPHANTOMTWINS_API AInteractHideObject : public ABaseObject, public IInteract
{
	GENERATED_BODY()

public:
	AInteractHideObject();


protected:
	virtual void BeginPlay() override;

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void OnInteractServer_Implementation(const APawn* Interactor) override;
	virtual void OnInteractClient_Implementation(const APawn* Interactor) override;
	virtual bool CanInteract_Implementation(const APawn* Interactor, bool bIsDetected) override;


public:
	// Ä«¸Þ¶ó
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "HideObject | Camera")
	TObjectPtr<class UCameraComponent> HideCameraComp;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HideObject")
    FHideState HideStatus;
};
