#include "BIKLevelBlock.h"
#include "BIKLevelBlock.h"

void FBIKLevelBlock::Tick(float DeltaTime)
{
	if (OnFireRemainingTime > 0.f)
	{
		OnFireRemainingTime -= DeltaTime;
		if (OnFireRemainingTime <= 0.f)
		{
			// Hide "Flames"
		}
	}
}

bool FBIKLevelBlock::IsOnFire() const
{
	return OnFireRemainingTime > 0.f;
}

bool FBIKLevelBlock::NeedsTick() const
{
	return IsOnFire();
}


