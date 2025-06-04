// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
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

UCLASS()
class THE_PHANTOM_TWINS_API APlayerBase : public ACharacter, public IPlayerWidgetInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerBase();

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

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collision")
	TObjectPtr<USphereComponent> SphereComponent;

	TArray<AActor*> InteractiveableObjects;

	virtual void NotifyActorBeginOverlap(AActor* Actor) override;
	virtual void NotifyActorEndOverlap(AActor* Actor) override;

public:
	void SetGroggy();

	UFUNCTION(BlueprintCallable, Category = "Stats")
	void TakeDamage(float Damage);

	virtual void SetupCharacterWidget(UMyPlayerUserWidget* UserWidget) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise")
	float RunNoise = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Noise")
	float WalkNoise = 1.0f;

protected:
	void ReferenceSetting();

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

public:


	bool bIsInventoryVisible = false;

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

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Run(const FInputActionValue& Value);
	void StopRun(const FInputActionValue& Value);
	void PlayerCrouch(const FInputActionValue& Value);
	void Hacking(const FInputActionValue& Value);
	void StopHacking(const FInputActionValue& Value);
	void Interactive(const FInputActionValue& Value);
	void OpenInventory(const FInputActionValue& Value);

	// NetWork
	UFUNCTION(Server, Reliable)
	void C2S_Interactive(UObject* interact);
	void C2S_Interactive_Implementation(UObject* interact);

	UFUNCTION(Server, Reliable)
	void C2S_Hacking(UObject* interact);
	void C2S_Hacking_Implementation(UObject* interact);

	UFUNCTION(Server, Reliable)
	void C2S_SetMaxWalkSpeed(float Speed);
	void C2S_SetMaxWalkSpeed_Implementation(float Speed);

	// Network
	UFUNCTION(Client, Reliable)
	void S2C_UpdatePerceivedActor(AActor* Actor, bool bVisible);
	void S2C_UpdatePerceivedActor_Implementation(AActor* Actort, bool bVisible);

};
