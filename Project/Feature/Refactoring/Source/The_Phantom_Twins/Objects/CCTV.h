// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gameframework/Pawn.h"
#include "SzInterface/Hacking.h"
#include "InputActionValue.h"
#include "CCTV.generated.h"

/**
 * 
 */
UCLASS()
class THE_PHANTOM_TWINS_API ACCTV : public APawn, public IHacking
{
	GENERATED_BODY()

public:
	ACCTV();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	virtual void OnInteractServer_Implementation(const APawn* Interactor) override;

	// 해킹
	virtual void OnHackingStartedServer_Implementation(const APawn* Interactor) override;
	virtual void OnHackingCompletedServer_Implementation(const APawn* Interactor) override;

	virtual bool CanBeHacked_Implementation(const APawn* Interactor) override;
	virtual void ClearHacking_Implementation(const APawn* Interactor) override;

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
	void SetWidgetVisible(bool Visible);
	void SetActorsOutlines(bool bActive);


public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BaseObject | Components")
	TObjectPtr<class USceneComponent> RootSceneComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BaseObject | Components")
	TObjectPtr<class UStaticMeshComponent> MeshComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BaseObject | Components")
	TObjectPtr<class USphereComponent> SphereCollisionComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BaseObject | Components")
	TObjectPtr<class UOutlineComponent> OutlineComp;


	// 가까운 오브젝트 확인용 위젯
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BaseObject | NearWidget")
	TObjectPtr<class UWidgetComponent> NearWidgetComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BaseObject | NearWidget")
	TSubclassOf<class UUserWidget> NearWidgetClass;
	
    // AI percrption(적이 사용)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BaseObject | AI")
    TObjectPtr<class UAIPerceptionStimuliSourceComponent> StimuliSource;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BaseObject")
	bool bCanInteract = true;

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

    UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "CCTV")
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
	TObjectPtr<const APawn> CurrentHackingPawn;

	UPROPERTY(ReplicatedUsing = OnRep_SetWidget, EditAnywhere, BlueprintReadWrite, Category = "CCTV")
	bool bSetWidgetDirtyFlag;
	/**
	 * @brief :
			클라이언트 코드도 처리하고 있음.
	 */
	UFUNCTION()
	void OnRep_SetWidget();

	UPROPERTY(ReplicatedUsing = OnRep_SetOutlines, EditAnywhere, BlueprintReadWrite, Category = "CCTV")
	bool bSetOutlineDirtyFlag;
	/**
	 * @brief :
			아웃라인
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