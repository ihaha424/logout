// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Utility/Filter_Mask.h"
#include "NavAreas/NavArea_Obstacle.h"

UFilter_Mask::UFilter_Mask()
{
    AddEnteringCostOverride(UNavArea_Obstacle::StaticClass(), 1.f);
}
