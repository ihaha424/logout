// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractiveComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class INTERACTTEST0424_API UInteractiveComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInteractiveComponent();

public:
	// 상호작용 처리 함수
    UFUNCTION(BlueprintCallable, Category = "Interaction")
    virtual void OnInteract(AActor* Interactor);

    // 디자이너가 설정 가능한 변수들
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction Settings")
    FString InteractionPrompt = TEXT("Press E to interact");
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction Settings")
    float InteractionDistance = 200.0f;


protected:
	// Called when the game starts
	virtual void BeginPlay() override;
};
