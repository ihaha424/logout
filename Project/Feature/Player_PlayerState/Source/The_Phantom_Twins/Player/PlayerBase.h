// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SzComponents/Interaction.h"
#include "PlayerWidgetInterface.h"
#include "InputActionValue.h"
#include "PlayerBase.generated.h"

class UPlayerStatComponent;
class UWidgetComponent;
class UCameraComponent;
class USpringArmComponent;
class UInputMappingContext;
class UInputAction;
class ABaseObject;
class UUserWidget;
class USphereComponent;
class APlayerDefaultState;

UCLASS()
class THE_PHANTOM_TWINS_API APlayerBase : public ACharacter, public IPlayerWidgetInterface, public IInteraction
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerBase();

	bool CheckActorInFront(AActor* TargetActor);

	void NearestObjectCheck();

	UFUNCTION(BlueprintImplementableEvent, Category = "Inventory")
	void AddItemToUI();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void PostInitializeComponents() override;

	virtual void OnRep_PlayerState() override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void OnInteractSever_Implementation(APawn* Player) override;
	virtual void OnInteractClient_Implementation(APawn* Player) override;
	virtual bool CanInteract_Implementation(const APawn* Player) const override;

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collision")
	TObjectPtr<USphereComponent> RangeSphere;

	UPROPERTY()
	TObjectPtr<APlayerDefaultState> PS;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collision")
	TObjectPtr<USphereComponent> RecoverySphere;

	TArray<AActor*> InteractiveableObjects;

	virtual void NotifyActorBeginOverlap(AActor* Actor) override;
	virtual void NotifyActorEndOverlap(AActor* Actor) override;

public:

	bool bIsInventoryOpen = true;

	UFUNCTION(BlueprintCallable, Category = "Stats")
	
	virtual float TakeDamage
	(
		float DamageAmount,
		struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator,
		AActor* DamageCauser
	) override;

	virtual void SetupCharacterWidget(UMyPlayerUserWidget* UserWidget) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<UCameraComponent> Camera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SpringArm")
	TObjectPtr<USpringArmComponent> SpringArm;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Object")
	TObjectPtr<UObject> NearestInteractiveObject;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TObjectPtr<UStaticMesh> InteractiveMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	TArray<UObject*> InventoryObjects;

	// Stat Section
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Stat)
	TObjectPtr<UPlayerStatComponent> Stat;

	// UI Widget Section
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Widget)
	TObjectPtr<UWidgetComponent> GroggyWidget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Widget)
	TObjectPtr<UUserWidget> InvenWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Widget)
	TSubclassOf<UUserWidget> InvenWidgetClass;

	// for using CCTV 
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Widget)
	//TObjectPtr<class UCCTVUserComponent> CCTVUserComp;

public:
	UPROPERTY()
	float NoiseTimer = 0.f;
	UPROPERTY()
	float MoveNoise = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise")
	float NoiseInterval = 3.f;
	UPROPERTY()
	float CurrentNoise = 100.f;

	// Input Section
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> InputMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> InteractiveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> RunAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> CrouchAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> HackingAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> InventoryAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> PhantomAction;

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Run(const FInputActionValue& Value);
	void StopRun(const FInputActionValue& Value);
	void PlayerCrouch(const FInputActionValue& Value);
	void Hacking(const FInputActionValue& Value);
	void StopHacking(const FInputActionValue& Value);
	void Interactive(const FInputActionValue& Value);
	void OpenInventory(const FInputActionValue& Value);
	void PhantomVision(const FInputActionValue& Value);

	// NetWork
	UFUNCTION(Server, Reliable)
	void C2S_Interactive(UObject* interact);
	void C2S_Interactive_Implementation(UObject* interact);

	UFUNCTION(Server, Reliable)
	void C2S_Hacking(UObject* interact);
	void C2S_Hacking_Implementation(UObject* interact);

	UFUNCTION(Server, Reliable)
	void C2S_StopHacking(UObject* interact);
	void C2S_StopHacking_Implementation(UObject* interact);

	UFUNCTION(Server, Reliable)
	void C2S_SetMaxWalkSpeed(float Speed);
	void C2S_SetMaxWalkSpeed_Implementation(float Speed);

	UFUNCTION(Server, Reliable)
	void C2S_MakeNoise(float Noise);
	void C2S_MakeNoise_Implementation(float Noise);

	void SetGroggy();
	void SetGroggyWidget(bool Visible);

	//UFUNCTION(NetMulticast, Reliable)
	void SetRecovery();
	//void S2A_SetRecovery_Implementation();

	UFUNCTION(Client, Reliable)
	void S2C_UpdatePerceivedActor(AActor* Actor, bool bVisible);
	void S2C_UpdatePerceivedActor_Implementation(AActor* Actort, bool bVisible);

private:
	void ReferenceSetting();
};
