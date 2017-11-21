#include "BIKExplosionActor.h"
#include "BIKLevelBlock.h"
#include "BIKDestructibleBlockActor.h"
#include "BIKPowerUpActor.h"

ABIKExplosionActor::ABIKExplosionActor()
{
	DelaySecondsToApply = 0.f;
	ExplosionSeconds = 0.f;
	DamageEnabled = true;

	//Enable tick for this class of actors
	PrimaryActorTick.bCanEverTick = true;
}

void ABIKExplosionActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (DelaySecondsToApply > 0.f)
	{
		DelaySecondsToApply -= DeltaSeconds;
		if (DelaySecondsToApply <= 0.f)
		{
			Explode();
		}
	}
	else if (ExplosionSeconds > 0.f)
	{
		ExplosionSeconds -= DeltaSeconds;
		if (ExplosionSeconds <= 0.f)
		{
			ExplosionEnd();
		}
	}
}

void ABIKExplosionActor::ConfigureExplosion(FBIKLevelBlock* LevelBlockArg, const float DelaySecondsArg, const float ExplosionSecondsArg)
{
	SetLevelBlock(LevelBlockArg);
	DelaySecondsToApply = DelaySecondsArg;
	ExplosionSeconds = ExplosionSecondsArg;
	// Hide the explosion until the DelaySeconds elapse
	if(DelaySecondsToApply > 0.f)
		SetActorHiddenInGame(true);
}

void ABIKExplosionActor::Explode()
{
	//Show the explosion
	SetActorHiddenInGame(false);
	// Destroy the block if there is a destructible on this space
	if (LevelBlock->IsDestroyable() && DamageEnabled)
	{
		// DestroyLevelBlock may disable the damage on this explosion if
		// it generates a powerup so we have to check again DamageEnabled
		LevelBlock->DestroyableBlock->DestroyLevelBlock();
	}

	if (LevelBlock->IsPowerUp() && DamageEnabled)
	{
		LevelBlock->PowerUp->Destroy();
		LevelBlock->PowerUp = nullptr;
	}
}

void ABIKExplosionActor::ExplosionEnd()
{
	// Update the level block where the explosion is standing and destroy this explosion actor
	LevelBlock->Explosion = nullptr;
	LevelBlock = nullptr;
	Destroy();
}