#pragma once

#include "BIKBlockPlaceableActor.h"
#include "BIKDestructibleBlockActor.generated.h"

UCLASS()
class ABIKDestructibleBlockActor : public ABIKBlockPlaceableActor
{
	GENERATED_BODY()

public:

	// Destroys block and generate powerup in place if needed
	void DestroyLevelBlock();
};
