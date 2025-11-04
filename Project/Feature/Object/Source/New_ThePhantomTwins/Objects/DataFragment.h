
#pragma once

#include "CoreMinimal.h"
#include "SzObjects/ItemObject.h"
#include "DataFragment.generated.h"

class UDataFragmentPickupWidget;

UCLASS()
class NEW_THEPHANTOMTWINS_API ADataFragment : public AItemObject
{
	GENERATED_BODY()
	
public:
	ADataFragment();

protected:
	virtual void BeginPlay() override;
	virtual void OnRep_bIsActived() override;

public:
	virtual void OnInteractServer_Implementation(const APawn* Interactor) override;
	virtual void DestroyItem() override;

	void SaveToGameState();
private:
	void SetDataFragmentPickupWidget();
	void HideFragmentMesh();

private:
	// 데이터조각을 먹으면 팝업으로 뜰 위젯
	UPROPERTY(EditDefaultsOnly, Category = "DataFragment | UI")
	TSubclassOf<UDataFragmentPickupWidget> DataFragmentPickupWidgetClass;

	UPROPERTY()
	TObjectPtr<UDataFragmentPickupWidget> DataFragmentPickupWidget;

	// 표시할 텍스트
	UPROPERTY(EditAnywhere, Category = "DataFragment | UI")
	FText DataFragmentText;

	// 위젯 표시 유지 시간
	UPROPERTY(EditAnywhere, Category = "DataFragment | UI")
	int32 WidgetDuration = 2;
public:
	UPROPERTY(EditAnywhere)
	TObjectPtr<USceneComponent> RootLocation;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USceneComponent> HostPlayerLocation;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USceneComponent> ClientPlayerLocation;

	UPROPERTY(EditAnywhere)
	TArray<TObjectPtr<USceneComponent>> PlayersLocation;
};