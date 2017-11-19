#pragma once

#include "CoreMinimal.h"
#include "BIKBlockPlaceableActor.h"
#include "BIKLevelBlock.h"
#include "BIKBombActor.generated.h"

UCLASS()
class ABIKBombActor : public ABIKBlockPlaceableActor
{
	GENERATED_BODY()

public:

	ABIKBombActor();

	/** Configure collision channels depending on which player fired the bomb */
	void ConfigureBomb(struct FBIKLevelBlock* LevelBlockArg, int32 BombBlockRadiusArg);

	/** Explodes the bomb destroying the bomb and setting in fire nearby level blocks*/
	void ExplodeBomb();

	/** Time in seconds since the bomb is planted until it explodes*/
	UPROPERTY(EditDefaultsOnly, Category = "BIKBombActor")
	float TimeToExplosionSeconds;

	virtual void Tick(float DeltaSeconds) override;

	FORCEINLINE int32 GetBombBlockRadius() const { return BombBlockRadius; }

private:

	/** Seconds remaining before explosion, updated each tick */
	float RemainingSeconds;

	/** Number of blocks the bomb will set on fire on each direction */
	int32 BombBlockRadius;
};
