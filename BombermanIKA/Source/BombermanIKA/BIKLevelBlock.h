#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BIKLevelBlock.generated.h"

USTRUCT()
struct FBIKLevelBlock
{
	GENERATED_BODY()

	/** Block that occupies this level block if exists */
	UPROPERTY()
	AActor* Block;

	/** Power-up actor over this block if exists*/
	UPROPERTY()
	AActor* PowerUp;

	/** "Flames" actor that must shown when this block is on fire due to a bomb*/
	UPROPERTY()
	AActor* Explosion;

	/** Bomb actor placed on this level block if exists*/
	UPROPERTY()
	AActor* Bomb;

	UPROPERTY()
	float OnFireRemainingTime;

	/** Checks if this block is on fire */
	bool IsOnExplosion() const;

	/** Called when a bomb is spawned */
	void BombSpawned(AActor* SpawnedBomb);

	/** Tells if there is a bomb on this level block */
	bool IsBombSpawned() const;

	/** Set the Level block on fire, normally called from the bomb that exploded */
	void ExplosionSpawned(AActor* ExplosionSpawned);

	/** Return the center of the block that comprises the given location */
	static FVector GetBlockCenterFromCurrentLocation(const FVector& Location);


	static const float BLOCK_SIZE;
	static const float HALF_BLOCK_SIZE;
};
