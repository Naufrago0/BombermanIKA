#include "BIKLevelBlock.h"
#include "BIKExplosionActor.h"

const float FBIKLevelBlock::BLOCK_SIZE = 100.f;
const float FBIKLevelBlock::HALF_BLOCK_SIZE = FBIKLevelBlock::BLOCK_SIZE * .5f;

bool FBIKLevelBlock::IsOnExplosion() const
{
	return Explosion != nullptr && !Explosion->bHidden;
}

bool FBIKLevelBlock::IsBombSpawned() const
{ 
	return Bomb != nullptr; 
}

bool FBIKLevelBlock::IsIndestructible() const
{
	return IndestructibleBlock != nullptr;
}

bool FBIKLevelBlock::IsDestroyable() const
{
	return DestroyableBlock != nullptr;
}

bool FBIKLevelBlock::IsPowerUp() const
{
	return PowerUp != nullptr;
}

void FBIKLevelBlock::ExplosionSpawned(ABIKExplosionActor* ExplosionSpawned)
{
	Bomb = nullptr; // If there is an explosion on the level block we can't have a bomb on it 
	Explosion = ExplosionSpawned;
}

FVector FBIKLevelBlock::GetBlockCenterFromCurrentLocation(const FVector& Location)
{
	FVector BlockCenter = Location;
	BlockCenter.X = FMath::TruncToFloat(BlockCenter.X / BLOCK_SIZE) * BLOCK_SIZE - HALF_BLOCK_SIZE;
	BlockCenter.Y = FMath::TruncToFloat(BlockCenter.Y / BLOCK_SIZE) * BLOCK_SIZE + HALF_BLOCK_SIZE;
	BlockCenter.Z = BLOCK_SIZE;

	return BlockCenter;
}

FVector FBIKLevelBlock::GetBlockCenterFromRowColIndexes(const int32 Row, const int32 Col)
{
	FVector BlockCenter;
	BlockCenter.X = Row * -BLOCK_SIZE - HALF_BLOCK_SIZE;
	BlockCenter.Y = Col * BLOCK_SIZE + HALF_BLOCK_SIZE;
	BlockCenter.Z = BLOCK_SIZE;
	return BlockCenter;
}

void FBIKLevelBlock::BombSpawned(ABIKBombActor* SpawnedBomb)
{
	Bomb = SpawnedBomb;
}