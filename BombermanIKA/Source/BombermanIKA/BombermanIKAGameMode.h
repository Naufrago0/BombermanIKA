// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BIKLevelBlock.h"
#include "BombermanIKAGameMode.generated.h"

#define LEVEL_WIDTH 27
#define LEVEL_HEIGHT 13

UCLASS(minimalapi)
class ABombermanIKAGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ABombermanIKAGameMode();

	virtual void BeginPlay() override;

	void GenerateLevel();

	UPROPERTY(EditDefaultsOnly)
	UClass* BlockActorClass;

protected:

	// Logic structure for all the maps
	FBIKLevelBlock LevelBlocksLogic[LEVEL_WIDTH][LEVEL_HEIGHT];

	// Array of level blocks that need to be updated
	TArray<FBIKLevelBlock*> UpdateBlocks;
};



