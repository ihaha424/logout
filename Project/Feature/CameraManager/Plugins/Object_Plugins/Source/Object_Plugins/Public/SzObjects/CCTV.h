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
	virtual void OnInteractClient_Implementation(APawn* Interactor) override;
	virtual bool CanInteract_Implementation(const APawn* Interactor) const override;
	virtual void SetWidgetVisibility_Implementation(bool Visible) override;


	// 해킹
	virtual void OnHackingStartedServer_Implementation(APawn* Interactor) override;
	virtual void OnHackingStartedClient_Implementation(APawn* Interactor) override;

	virtual void OnHackingCompletedServer_Implementation(APawn* Interactor) override;
	virtual void OnHackingCompletedClient_Implementation(APawn* Interactor) override;

	virtual bool CanBeHacked_Implementation() const override;
	virtual void ClearHacking_Implementation() override;

	// Input 콜백
	void Turn(const FInputActionValue& Value);
	void Exit(const FInputActionValue& Value);
	void Prev(const FInputActionValue& Value);
	void Next(const FInputActionValue& Value);

	int32 GetID() { return CCTVID; }

	// 상태 관리 함수
	void EnterCCTVView(APlayerController* PlayerController);
	void ExitCCTVView(APlayerController* PlayerController);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	void SetActorsOutlines(bool bActive);


public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CCTV")
	TObjectPtr<class USpringArmComponent> SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CCTV")
	TObjectPtr<class UCameraComponent> CameraComp;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "CCTV")
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
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	TObjectPtr<class UInputMappingContext> InputMappingContext;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<class UInputMappingContext> PlayerMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	TObjectPtr<class UInputAction> IA_Turn;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	TObjectPtr<class UInputAction> IA_Exit;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	TObjectPtr<class UInputAction> IA_Prev;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Input")
	TObjectPtr<class UInputAction> IA_Next;

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

	UPROPERTY(EditAnywhere, Category = "CCTV | Hacking")
	TObjectPtr<class UNoiseComponent> NoiseComp;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CCTV")
	int32 CCTVID = 0;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UUserWidget> PhantomVisionWidget;
	
	UPROPERTY()
	TObjectPtr<class UUserWidget> PhantomVisionUI;

	// 현재 해킹 중인 플레이어를 추적하기 위한 변수 추가
	UPROPERTY()
	TObjectPtr<APawn> CurrentHackingPawn;

	UPROPERTY(ReplicatedUsing = OnRep_SetOutlines, EditAnywhere, BlueprintReadWrite, Category = "CCTV")
	bool bSetOutlinesDirtyFlag;
	/**
	 * @brief :
			임시로 아웃라인뿐만 아니라 클라이언트 코드도 처리하고 있음.
	 */
	UFUNCTION()
	void OnRep_SetOutlines();

	UFUNCTION(Server, Reliable)
	void C2S_Exit();
	void C2S_Exit_Implementation();

	UFUNCTION(Server, Reliable)
	void C2S_ChangeCCTV(ACCTV* nextCCTV);
	void C2S_ChangeCCTV_Implementation(ACCTV* nextCCTV);

};