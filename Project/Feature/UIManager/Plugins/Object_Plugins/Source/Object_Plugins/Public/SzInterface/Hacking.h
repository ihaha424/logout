// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Hacking.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UHacking : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class OBJECT_PLUGINS_API IHacking
{
	GENERATED_BODY()

public:
	// 해킹 실행 (E키 홀딩)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Hacking")
	void OnHackingStartedServer(APawn* Interactor);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Hacking")
	void OnHackingStartedClient(APawn* Interactor);

	// 해킹 완료 후 로직 (CCTV 보임, 적 무력화 등)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Hacking")
	void OnHackingCompletedServer(APawn* Interactor);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Hacking")
	void OnHackingCompletedClient(APawn* Interactor);

	// 해킹 가능 여부 체크 (false => 해킹 완료 / true => 해킹 전)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Hacking")
	bool CanBeHacked() const;

	// 해킹 초기화
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Hacking")
	void ClearHacking();

	UFUNCTION(BlueprintNativeEvent, Category = "Interaction")
	void SetWidgetVisibility(bool Visible);
};


