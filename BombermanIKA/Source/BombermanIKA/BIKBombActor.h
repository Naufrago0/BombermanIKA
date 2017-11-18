#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BIKLevelBlock.h"
#include "BIKBombActor.generated.h"

UCLASS()
class ABIKBombActor : public AActor
{
	GENERATED_BODY()

public:

	ABIKBombActor();

	/** Configure collision channels depending on which player fired the bomb */
	void ConfigureBomb(struct FBIKLevelBlock* LevelBlockArg);

	/** Explodes the bomb destroying the bomb and setting in fire nearby level blocks*/
	void ExplodeBomb();

	/** Time in seconds since the bomb is planted until it explodes*/
	UPROPERTY(EditDefaultsOnly, Category = "BIKBombActor")
	float TimeToExplosionSeconds;

	virtual void Tick(float DeltaSeconds) override;

private:
	FBIKLevelBlock* LevelBlock;

	/** Seconds remaining before explosion, updated each tick */
	float RemainingSeconds;
};
