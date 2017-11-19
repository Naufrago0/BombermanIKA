#include "BIKPowerUpActor.h"
#include "BIKLevelBlock.h"

void ABIKPowerUpActor::ConfigurePowerUp(FBIKLevelBlock* LevelBlockArg, EPowerUpType TypeArg)
{
	SetLevelBlock(LevelBlockArg);
	PowerUpType = TypeArg;
}

void ABIKPowerUpActor::ApplyPowerUpToPlayer(ABombermanIKAPlayerController* BIKPC)
{
	check(BIKPC != nullptr);
	BIKPC->ApplyPowerUp(PowerUpType);
}

void ABIKPowerUpActor::TakePowerUp(ABombermanIKAPlayerController* BIKPC)
{
	check(BIKPC != nullptr);
	BIKPC->ApplyPowerUp(PowerUpType);
	LevelBlock->PowerUp = nullptr;
	LevelBlock = nullptr;
	Destroy();
}