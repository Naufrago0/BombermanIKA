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

protected:
	FVector AccumulatedMovement;

	bool bIsFirstPlayer;

	bool bIsAlive;

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

};


