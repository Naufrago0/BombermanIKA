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

	/** Spawn a bomb for the specified player controller, return true if the bomb was successfully planted, false elsewhere */
	bool SpawnBombForPlayer(class ABombermanIKAPlayerController* PC);

	/** Process bomb explosion, creates explosion actors and update level accrodingly*/
	void ExplodeBomb(class ABIKBombActor* ExplodedBomb);

	/** Spawn a random powerup with a configured probability */
	void SpawnRandomPowerUp(const FVector& Location);

	FBIKLevelBlock* GetBlockFromLocation(const FVector& Position);

	void GetBlockIndexesFromLocation(const FVector& Position, int32& Row, int32& Column);

	/** Default class for blocking undestructible blocks */
	UPROPERTY(EditDefaultsOnly, Category = "BombermanIKA")
	UClass* BlockActorClass;

	/** Default class for destructible blocks */
	UPROPERTY(EditDefaultsOnly, Category = "BombermanIKA")
	UClass* DestructibleBlockActorClass;

	/** Default actor class for bombs */
	UPROPERTY(EditDefaultsOnly, Category = "BombermanIKA")
	UClass* BombActorClass;

	/** Default class for explosion actor*/
	UPROPERTY(EditDefaultsOnly, Category = "BombermanIKA")
	UClass* ExplosionActorClass;

	/** Default class for power up that increases explosion radius*/
	UPROPERTY(EditDefaultsOnly, Category = "BombermanIKA")
	UClass* ExplosionRadiusPowerUpActorClass;

	/** Default class for power up that increases number of available bombs*/
	UPROPERTY(EditDefaultsOnly, Category = "BombermanIKA")
	UClass* ExtraBombPowerUpActorClass;

	/** Default class for power up that enables remote controller for placed bombs*/
	UPROPERTY(EditDefaultsOnly, Category = "BombermanIKA")
	UClass* RemoteControlPowerUpActorClass;

	/** Default class for power up that enables remote controller for placed bombs*/
	UPROPERTY(EditDefaultsOnly, Category = "BombermanIKA")
	UClass* SpeedUpPowerUpActorClass;

	/** Default time in seconds an explosion lasts */
	UPROPERTY(EditDefaultsOnly, Category = "BombermanIKA")
	float DefaultExplosionSeconds;

	/** Default delay in seconds to spawn explosion for each block of distance to the originating bomb*/
	UPROPERTY(EditDefaultsOnly, Category = "BombermanIKA")
	float DefaultDelaySecondsPerBlock;

	/** Probability of a destructible block to spawn a powerup */
	UPROPERTY(EditDefaultsOnly, Category = "BombermanIKA", meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float SpawnPowerUpProbability;

	/** Number of destructible blocks to create when generating the level */
	UPROPERTY(EditDefaultsOnly, Category = "BombermanIKA")
	int32 DestructibleBlocks;

protected:

	// Logic structure for all the maps, each block tells what is on it
	FBIKLevelBlock LevelBlocksLogic[LEVEL_WIDTH][LEVEL_HEIGHT];
};



