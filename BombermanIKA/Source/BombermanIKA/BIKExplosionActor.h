#pragma once

#include "CoreMinimal.h"
#include "BIKBlockPlaceableActor.h"
#include "BIKExplosionActor.generated.h"

UCLASS()
class ABIKExplosionActor : public ABIKBlockPlaceableActor
{
	GENERATED_BODY()

public:

	ABIKExplosionActor();

	void Tick(float DeltaSeconds) override;

	void ConfigureExplosion(struct FBIKLevelBlock* LevelBlock, const float DelaySecondsArg, const float ExplosionSecondsArg);

	FORCEINLINE void SetExplosionSeconds(const float ExplosionSecondsArg) { ExplosionSeconds = ExplosionSecondsArg; }

	FORCEINLINE void DisableDamage() { DamageEnabled = false; }

private:

	/** Once the delay time is elapsed this is called to show the explosion and start to apply damage */
	void Explode();

	/** Once the explosion time is elapsed this is called to destroy the explosion actor */
	void ExplosionEnd();

	/** Delay time from bomb explosion to start to apply this explosion, this can be used to fake the explosion spreading*/
	float DelaySecondsToApply;

	/** Time the explosion will last*/
	float ExplosionSeconds;

	/** Only If true the explosion makes damage */
	bool DamageEnabled;
};
