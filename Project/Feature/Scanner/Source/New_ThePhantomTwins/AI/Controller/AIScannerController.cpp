// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Controller/AIScannerController.h"

AAIScannerController::AAIScannerController()
{
	StimulusPriorityMap.Empty();

	StimulusPriorityMap = {
		{ "PlayerActor",	{1, 1} },
		{ "EnemyActor",		{1, 1} },
		{ "NoiseItem",		{2, 100} },
		{ "ScanPlayer",		{3, 1} },
		{ "EnemyRun",		{4, 40} },
		{ "EnemyWalk",		{5, 20} },
		{ "PlayerRun",		{6, 40} },
		{ "PlayerWalk",		{7, 20} },
	};

}
