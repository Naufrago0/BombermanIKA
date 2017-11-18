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
	/**
	 * Overriden to configure the player controller to be the first player's player controller or the second player's player controller using the Portal string.
	 * The portal string is used to differentiate players because the game mode finds APlayerStart actors using this portal string, so we can assign each character
	 * to its initial position easily and also this player controller identifying before initialization allows us to configure proper keyboard bindings for each player.
	 */
	virtual FString InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal = TEXT("")) override;

	/**
	 * Overriden to identify first or second player using Portal string
	 */
	virtual APlayerController* Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal, const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;

	void GenerateLevel();

	UPROPERTY(EditDefaultsOnly)
	UClass* BlockActorClass;

protected:

	// Logic structure for all the maps
	FBIKLevelBlock LevelBlocksLogic[LEVEL_WIDTH][LEVEL_HEIGHT];

	// Array of level blocks that need to be updated
	TArray<FBIKLevelBlock*> UpdateBlocks;
};



