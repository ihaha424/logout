// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MyPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class INTERACTTEST0424_API AMyPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	AMyPlayerController();

public:
	// 가장 가까운 상호작용 가능 오브젝트 
    UPROPERTY(BlueprintReadOnly, Category = "Interaction")
    TObjectPtr<AActor> NearestInteractableActor;
    
    // 상호작용 거리
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
    float InteractionDistance = 250.0f;

protected:
    virtual void SetupInputComponent() override;
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    // 상호작용 키 입력 처리
    void HandleInteractionInput();
    
    // 주변 상호작용 가능 오브젝트 검색
    void FindNearestInteractableActor();
};
