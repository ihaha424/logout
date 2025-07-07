#pragma once

#include "CoreMinimal.h"
#include "ObjectState.generated.h"


USTRUCT(BlueprintType)
struct FItemState
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsPickedUp = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bDestory = false;
};


USTRUCT(BlueprintType)
struct FHideState
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hide")
	bool bHasPlayer = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hide")
	int32 PlayerNum = 0;
};