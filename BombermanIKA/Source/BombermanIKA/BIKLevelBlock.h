#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BIKLevelBlock.generated.h"

USTRUCT()
struct FBIKLevelBlock
{
	GENERATED_BODY()

	/** Undestructible Block that occupies this level block if exists */
	UPROPERTY()
	AActor* IndestructibleBlock;

	/** Destructible Block that occupies this level block if exists */
	UPROPERTY()
	class ABIKDestructibleBlockActor* DestroyableBlock;

	/** Power-up actor over this block if exists*/
	UPROPERTY()
	class ABIKPowerUpActor* PowerUp;

	/** "Flames" actor that must shown when this block is on fire due to a bomb*/
	UPROPERTY()
	class ABIKExplosionActor* Explosion;

	/** Bomb actor placed on this level block if exists*/
	UPROPERTY()
	class ABIKBombActor* Bomb;

	/** Checks if this block is on fire */
	bool IsOnExplosion() const;

	/** Called when a bomb is spawned */
	void BombSpawned(ABIKBombActor* SpawnedBomb);

	/** Tells if there is a bomb on this level block */
	bool IsBombSpawned() const;

	/** Tells if in this block there is an undestructible block */
	bool IsIndestructible() const;

	/** Tells if in this block there is a destructible block*/
	bool IsDestroyable() const;

	/** Tells if in this block there is a power up standing*/
	bool IsPowerUp() const;

	/** Set the Level block on fire, normally called from the bomb that exploded */
	void ExplosionSpawned(ABIKExplosionActor* ExplosionSpawned);

	/** Return the center of the block that comprises the given location */
	static FVector GetBlockCenterFromCurrentLocation(const FVector& Location);

	/** Return the center of the block given by the row and col coordinates */
	static FVector GetBlockCenterFromRowColIndexes(const int32 Row, const int32 Col);

	static const float BLOCK_SIZE;
	static const float HALF_BLOCK_SIZE;
};
