#include "BIKDestructibleBlockActor.h"
#include "BIKLevelBlock.h"
#include "BombermanIKAGameMode.h"

void ABIKDestructibleBlockActor::DestroyLevelBlock()
{
	auto GameMode = Cast<ABombermanIKAGameMode>(GetWorld()->GetAuthGameMode());
	GameMode->SpawnRandomPowerUp(GetActorLocation());
	LevelBlock->DestroyableBlock = nullptr;
	LevelBlock = nullptr;
	Destroy();
}