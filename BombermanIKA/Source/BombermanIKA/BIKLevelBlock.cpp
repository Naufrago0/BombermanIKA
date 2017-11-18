#include "BIKLevelBlock.h"
#include "BIKLevelBlock.h"

const float FBIKLevelBlock::BLOCK_SIZE = 100.f;
const float FBIKLevelBlock::HALF_BLOCK_SIZE = FBIKLevelBlock::BLOCK_SIZE * .5f;

bool FBIKLevelBlock::IsOnExplosion() const
{
	return Explosion != nullptr;
}

bool FBIKLevelBlock::IsBombSpawned() const
{ 
	return Bomb != nullptr; 
}

void FBIKLevelBlock::ExplosionSpawned(AActor* ExplosionSpawned)
{
	Bomb = nullptr; // If there is an explosion on the level block we can't have a bomb on it 
	Explosion = ExplosionSpawned;
}

FVector FBIKLevelBlock::GetBlockCenterFromCurrentLocation(const FVector& Location)
{
	FVector BlockCenter = Location;
	BlockCenter.X = FMath::TruncToFloat(BlockCenter.X / BLOCK_SIZE) * BLOCK_SIZE - HALF_BLOCK_SIZE;
	BlockCenter.Y = FMath::TruncToFloat(BlockCenter.Y / BLOCK_SIZE) * BLOCK_SIZE + HALF_BLOCK_SIZE;

	return BlockCenter;
}

void FBIKLevelBlock::BombSpawned(AActor* SpawnedBomb)
{
	Bomb = SpawnedBomb;
}