#include "BIKDestructibleBlockActor.h"
#include "BIKLevelBlock.h"
#include "BIKExplosionActor.h"
#include "BombermanIKAGameMode.h"

void ABIKDestructibleBlockActor::DestroyLevelBlock()
{
	auto GameMode = Cast<ABombermanIKAGameMode>(GetWorld()->GetAuthGameMode());
	//Disable explosion damage so it doesn't destroys the powerup if one is generated
	if (LevelBlock->IsOnExplosion())
	{
		LevelBlock->Explosion->DisableDamage();
	}

	GameMode->SpawnRandomPowerUp(GetActorLocation());
	LevelBlock->DestroyableBlock = nullptr;
	LevelBlock = nullptr;
	Destroy();
}