#pragma once

#include "BIKBlockPlaceableActor.h"
#include "BombermanIKAPlayerController.h"
#include "BIKPowerUpActor.generated.h"

UCLASS()
class ABIKPowerUpActor : public ABIKBlockPlaceableActor
{
	GENERATED_BODY()

public:

	void ConfigurePowerUp(FBIKLevelBlock* LevelBlockArg, EPowerUpType TypeArg);

	/** Apply the configured powerup to the player controller*/
	void ApplyPowerUpToPlayer(ABombermanIKAPlayerController* BIKPC);

	/** Called by player controllers to inform power-ups they have been taken */
	void TakePowerUp(ABombermanIKAPlayerController* BIKPC);

private:

	EPowerUpType PowerUpType;
};
