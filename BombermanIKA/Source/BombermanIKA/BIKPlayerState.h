#pragma once

#include "GameFramework/PlayerState.h"
#include "BIKPlayerState.generated.h"

UCLASS()
class ABIKPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	UPROPERTY()
	int32 RoundsScore;

	FORCEINLINE void IncreaseScore() { ++RoundsScore; }
};

