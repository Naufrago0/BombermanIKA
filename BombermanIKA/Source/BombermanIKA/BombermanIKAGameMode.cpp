// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "BombermanIKAGameMode.h"
#include "BombermanIKAPlayerController.h"
#include "BombermanIKACharacter.h"
#include "UObject/ConstructorHelpers.h"

ABombermanIKAGameMode::ABombermanIKAGameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = ABombermanIKAPlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/TopDownCPP/Blueprints/TopDownCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

void ABombermanIKAGameMode::BeginPlay()
{
	GenerateLevel();
}

void ABombermanIKAGameMode::GenerateLevel()
{
	for (int32 i = 0; i < LEVEL_WIDTH; ++i)
	{
		for (int32 j = 0; j < LEVEL_HEIGHT; ++j)
		{
			if ((i & 1) && (j & 1))
			{
				FVector Location(j * -100.f - 50.f, i * 100.f + 50.f, 100.f);
				LevelBlocksLogic[i][j].Block = GetWorld()->SpawnActor(BlockActorClass, &Location);
			}
		}
	}
}