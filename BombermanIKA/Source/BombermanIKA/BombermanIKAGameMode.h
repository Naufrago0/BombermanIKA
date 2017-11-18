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

	/**
	 * Construct level and spawn players
	 */
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

	/** Generates a new level */
	void GenerateLevel();

	/** Spawn a bomb for the specified player controller */
	void SpawnBombForPlayer(class ABombermanIKAPlayerController* PC);

	FBIKLevelBlock* GetBlockFromLocation(const FVector& Position);

	/** Default class for blocking undestructible blocks */
	UPROPERTY(EditDefaultsOnly, Category = "BombermanIKA")
	UClass* BlockActorClass;

	/** Default class for destructible blocks */
	UPROPERTY(EditDefaultsOnly, Category = "BombermanIKA")
	UClass* DestructibleBlockActorClass;

	/** Default actor class for bombs */
	UPROPERTY(EditDefaultsOnly, Category = "BombermanIKA")
	UClass* BombActorClass;

protected:

	// Logic structure for all the maps, each block tells what is on it
	FBIKLevelBlock LevelBlocksLogic[LEVEL_WIDTH][LEVEL_HEIGHT];
};



