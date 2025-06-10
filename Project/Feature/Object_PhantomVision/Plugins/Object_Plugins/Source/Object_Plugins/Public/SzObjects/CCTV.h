// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SzObjects/BaseObject.h"
#include "SzInterface/Hacking.h"
#include "InputActionValue.h"
#include "CCTV.generated.h"

/**
 * 
 */
UCLASS()
class OBJECT_PLUGINS_API ACCTV : public ABaseObject, public IHacking
{
	GENERATED_BODY()

public:
	ACCTV();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	virtual void OnInteractSever_Implementation(APawn* Interactor) override;
	virtual bool CanInteract_Implementation(const APawn* Interactor) const override;
	virtual void SetWidgetVisibility_Implementation(bool Visible) override;


	// 해킹
	virtual void OnHackingStarted_Implementation(APawn* Interactor) override;
	virtual void OnHackingCompleted_Implementation(APawn* Interactor) override;
	virtual bool CanBeHacked_Implementation() const override;
	virtual void ClearHacking_Implementation() override;

	// Input 콜백
	void Turn(const FInputActionValue& Value);
	void Exit(const FInputActionValue& Value);

private:
	// 상태 관리 함수
	void EnterCCTVView(APlayerController* PlayerController);
	void ExitCCTVView(APlayerController* PlayerController);

public:
	// CCTV 설정
	UPROPERTY(EditAnywhere, Category = "CCTV")
	TObjectPtr<AActor> RequiredKey;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CCTV")
	bool bHasKey = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CCTV")
	TObjectPtr<class USpringArmComponent> SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CCTV")
	TObjectPtr<class UCameraComponent> CameraComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CCTV")
	bool bIsInCCTVView = false;

	UPROPERTY()
	TObjectPtr<AActor> PreviousViewTarget;

	UPROPERTY()
	TObjectPtr<APawn> PreviousPawn;

	// 회전 제한
	UPROPERTY(EditAnywhere, Category = "CCTV|Camera Limit", meta = (ClampMin = "0.0", ClampMax = "90.0"))
	float MaxPitch = 15.0f;

	UPROPERTY(EditAnywhere, Category = "CCTV|Camera Limit", meta = (ClampMin = "0.0", ClampMax = "180.0"))
	float MaxYaw = 40.0f;

	// 입력 설정
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<class UInputMappingContext> InputMappingContext;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<class UInputMappingContext> PlayerMappingContext;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<class UInputAction> IA_Turn;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<class UInputAction> IA_Exit;

	TObjectPtr<class UEnhancedInputLocalPlayerSubsystem> InputSubsystem;

	UPROPERTY()
	FRotator BaseControlRotation;

	// CardKey
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CCTV")
    int32 SecurityLevel = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CCTV")
    bool IsActive = false;

	// 해킹
	UPROPERTY(EditAnywhere, Category = "CCTV | Hacking")
	TObjectPtr<class UHackableComponent> HackingComp;
};