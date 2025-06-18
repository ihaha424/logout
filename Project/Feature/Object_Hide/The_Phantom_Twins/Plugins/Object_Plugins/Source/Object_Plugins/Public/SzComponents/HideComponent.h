#pragma once

#include "CoreMinimal.h"
#include "InteractableComponent.h"
#include "HideComponent.generated.h"


UCLASS( ClassGroup=(Interaction), meta=(BlueprintSpawnableComponent) )
class OBJECT_PLUGINS_API UHideComponent : public UInteractableComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHideComponent();

protected:
	// Called when the game starts
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay() override;

public:
	virtual void ExecuteSever(APawn* Interactor) override;
	virtual void ExecuteClient(APawn* Interactor) override;
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Hide", Replicated)
    bool bHasPlayer = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	TObjectPtr<class UBoxComponent> TriggerComponent;

	// 카메라 사용 여부
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera", meta=(InlineEditConditionToggle), Replicated)
    bool bUseCamera = true;  // 기본값 true (카메라 사용)

	// 카메라 컴포넌트 참조
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (EditCondition = "bUseCamera"))
	TObjectPtr<class UCameraComponent> HideCamera;

	// 카메라 전환을 위한 추가 변수
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", Replicated)
	bool bIsInHideView = false;

	// 블렌드 시간 설정
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float CameraBlendTime = 0.75f;

	// 이전 뷰 타겟 저장
	UPROPERTY(Replicated)
	TObjectPtr <AActor> PreviousViewTarget = nullptr;

protected:
	void NoCamLogic(APlayerController* InteractorPC);

	void UseCamLogic(APlayerController* InteractorPC);
	void ClientUseCamLogic(APlayerController* InteractorPC);

	void EnterObject(APlayerController* InteractorPC);
	void ExitObject(APlayerController* InteractorPC);

	void SetInputState(APlayerController* InteractorPC, bool bIgnoreInput);		// 입력 제어를 위한 함수
	void SetViewTarget(APlayerController* InteractorPC, AActor* NewViewTarget); // 카메라 전환을 위한 함수


	UFUNCTION()
    void OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);

    UFUNCTION()
    void OnTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


private:
	UPROPERTY(Replicated)
	TObjectPtr<APlayerController> HidePlayer = nullptr;
};