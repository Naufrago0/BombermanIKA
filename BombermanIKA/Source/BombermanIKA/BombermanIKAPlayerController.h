// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BombermanIKAPlayerController.generated.h"

UCLASS()
class ABombermanIKAPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ABombermanIKAPlayerController();

	FORCEINLINE void SetFirstPlayer(bool bIsFirstPlayerArg) { bIsFirstPlayer = bIsFirstPlayerArg; }
	FORCEINLINE bool IsFirstPlayer() const { return bIsFirstPlayer; }

	void OnBombExploded();

	// Gameplay utility methods
	FORCEINLINE int32 GetBombBlockRadius() const { return BombBlockRadius; }

protected:
	FVector AccumulatedMovement;

	bool bIsFirstPlayer;

	bool bIsAlive;

	// Number of bombs this player can fire
	int32 Bombs;

	// Maximum Number of bombs this player can have
	int32 BombsUpperLimit;

	// Maximum number of bombs defined for the game
	static const int32 MaxBombs;

	// Explosion radius in blocks that the bombs spawned by this player has.
	int32 BombBlockRadius;

	// Begin PlayerController interface
	virtual void PlayerTick(float DeltaTime) override;
	virtual void SetupInputComponent() override;
	virtual void Possess(class APawn* aPawn) override;
	// End PlayerController interface

	// Movement management
	void OnUpPressed();
	void OnUpReleased();
	void OnLeftPressed();
	void OnLeftReleased();
	void OnDownPressed();
	void OnDownReleased();
	void OnRightPressed();
	void OnRightReleased();

	// Firing management

	void OnBombFired();
};


