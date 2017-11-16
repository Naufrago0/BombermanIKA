#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BIKLevelBlock.generated.h"

USTRUCT()
struct FBIKLevelBlock
{
	GENERATED_BODY()

	/** Block that occupies this level block if exists */
	UPROPERTY()
	AActor* Block;

	/** Power-up actor over this block if exists*/
	UPROPERTY()
	AActor* PowerUp;

	/** "Flames" actor that must shown when this block is on fire due to a bomb*/
	UPROPERTY()
	AActor* OnFire;

	UPROPERTY()
	float OnFireRemainingTime;

	// Updates block state
	void Tick(float DeltaTime);

	// Checks if this block is on fire
	bool IsOnFire() const;

	// Tells if the block needs update
	bool NeedsTick() const;

};
