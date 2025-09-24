// Fill out your copyright notice in the Description page of Project Settings.


#include "BT_TPT_CompositeNode.h"

uint8* UBT_TPT_CompositeNode::GetCompositeNodeMemory(FBehaviorTreeSearchData& SearchData, const UBTNode* Node, int32 InstanceIdx) const
{
	uint8* BaseMemory = NULL;
	if (bUseInstanceMemory)
	{
		BaseMemory = (uint8*)SearchData.OwnerComp.GetNodeMemory(Node, InstanceIdx);
		BaseMemory += sizeof(FBTCompositeMemory);
	}
	return BaseMemory;
}

uint16 UBT_TPT_CompositeNode::GetMyInstanceMemorySize(uint16 size) const
{
	bUseInstanceMemory = true;
	return Super::GetInstanceMemorySize() + size;
}
