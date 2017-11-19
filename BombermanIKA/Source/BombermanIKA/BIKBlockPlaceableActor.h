#pragma once

#include "GameFramework/Actor.h"
#include "BIKBlockPlaceableActor.generated.h"

UCLASS()
class ABIKBlockPlaceableActor : public AActor
{
	GENERATED_BODY()

public:
	
	ABIKBlockPlaceableActor();

	FORCEINLINE struct FBIKLevelBlock* GetLevelBlock() const { return LevelBlock; }

protected:
	/** Block where the actor is placed on*/
	FBIKLevelBlock* LevelBlock;
};
