// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTCompositeNode.h"
#include "BT_TPT_CompositeNode.generated.h"

/**
 * 
 */
UCLASS()
class NEW_THEPHANTOMTWINS_API UBT_TPT_CompositeNode : public UBTCompositeNode
{
	GENERATED_BODY()
protected:
	uint8* GetCompositeNodeMemory(FBehaviorTreeSearchData& SearchData, const UBTNode* Node, int32 InstanceIdx) const;

	/**
	 *	If you want to use InstanceMemory, call Super::GetInstanceMemorySize()
			ex> Super::GetInstanceMemorySize(Your Instance Memory Size);
	* @param Node
			: Your Instance Memory Size
	* @return
			: Total Memory Size
	 */
	uint16 GetMyInstanceMemorySize(uint16 size) const;
private:
	mutable bool bUseInstanceMemory = false;
};
