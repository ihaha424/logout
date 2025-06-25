// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIInterface.h"
#include "GameFramework/Character.h"
#include "SzInterface/Hacking.h"
#include "MyAICharacter.generated.h"

UCLASS()
class THE_PHANTOM_TWINS_API AMyAICharacter : public ACharacter, public IHacking
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMyAICharacter();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* JogFwdMontage;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastPlayJogFwdMontage(float PlayRate = 1.0f);

	void PlayJogFwdMontageSynced(float PlayRate = 1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	UAnimMontage* JumpMontage;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastPlayJumpMontage(float PlayRate = 1.0f);

	void PlayJumpMontageSynced(float PlayRate = 1.0f);


	virtual void OnHackingStartedServer_Implementation(APawn* Interactor) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Spline")
	class ASplinePathActor* BaseSplinePath;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Spline")
	class ASplinePathActor* StimulusSplinePath;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	class UWidgetComponent* AIStateWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TObjectPtr<class UWidgetComponent> WidgetComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class USphereComponent> SphereCollisionComp;

	UFUNCTION(NetMulticast, Reliable)
	void S2A_UpdateAIStateWidget(EAIStateWidget State);
	void S2A_UpdateAIStateWidget_Implementation(EAIStateWidget State);

	UFUNCTION(NetMulticast, Reliable)
	void S2A_UpdateWidgetDirection(FRotator Rotate);
	void S2A_UpdateWidgetDirection_Implementation(FRotator Rotate);

	float DistanceAlongSpline = 0.f;
	bool bMovingForward = true;

	UPROPERTY(EditAnywhere, Category = "AI|Spline")
	float MoveSpeed = 200.f;

	bool bIsFollowingSpline = false;

	UFUNCTION(CallInEditor, BlueprintCallable, Category = "AI|Dead")
	virtual void SetDead();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	float DeadEventDelayTime = 2.0f;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void SetWidgetVisibility_Implementation(bool Visible) override;
	class ASplinePathActor* GetBaseSplinePath() const { return BaseSplinePath; }
	
};
