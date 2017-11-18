// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "BombermanIKAPlayerController.h"
#include "AI/Navigation/NavigationSystem.h"
#include "Runtime/Engine/Classes/Components/DecalComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "BombermanIKACharacter.h"

ABombermanIKAPlayerController::ABombermanIKAPlayerController()
{
	bShowMouseCursor = false;
	bIsFirstPlayer = true;
	bIsAlive = true;
}

void ABombermanIKAPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	// keep updating the destination every tick while desired
	if (AccumulatedMovement != FVector::ZeroVector)
	{
		APawn* Pawn = GetPawn();
		if (Pawn != nullptr)
			Pawn->AddMovementInput(AccumulatedMovement);
	}
}

void ABombermanIKAPlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	// Player 1 Setup

	if (bIsFirstPlayer)
	{

		InputComponent->BindAction("MoveUpP1", IE_Pressed, this, &ABombermanIKAPlayerController::OnUpPressed);
		InputComponent->BindAction("MoveLeftP1", IE_Pressed, this, &ABombermanIKAPlayerController::OnLeftPressed);
		InputComponent->BindAction("MoveDownP1", IE_Pressed, this, &ABombermanIKAPlayerController::OnDownPressed);
		InputComponent->BindAction("MoveRightP1", IE_Pressed, this, &ABombermanIKAPlayerController::OnRightPressed);

		InputComponent->BindAction("MoveUpP1", IE_Released, this, &ABombermanIKAPlayerController::OnUpReleased);
		InputComponent->BindAction("MoveLeftP1", IE_Released, this, &ABombermanIKAPlayerController::OnLeftReleased);
		InputComponent->BindAction("MoveDownP1", IE_Released, this, &ABombermanIKAPlayerController::OnDownReleased);
		InputComponent->BindAction("MoveRightP1", IE_Released, this, &ABombermanIKAPlayerController::OnRightReleased);
	}

	// Player 2 Setup

	else
	{
		InputComponent->BindAction("MoveUpP2", IE_Pressed, this, &ABombermanIKAPlayerController::OnUpPressed);
		InputComponent->BindAction("MoveLeftP2", IE_Pressed, this, &ABombermanIKAPlayerController::OnLeftPressed);
		InputComponent->BindAction("MoveDownP2", IE_Pressed, this, &ABombermanIKAPlayerController::OnDownPressed);
		InputComponent->BindAction("MoveRightP2", IE_Pressed, this, &ABombermanIKAPlayerController::OnRightPressed);

		InputComponent->BindAction("MoveUpP2", IE_Released, this, &ABombermanIKAPlayerController::OnUpReleased);
		InputComponent->BindAction("MoveLeftP2", IE_Released, this, &ABombermanIKAPlayerController::OnLeftReleased);
		InputComponent->BindAction("MoveDownP2", IE_Released, this, &ABombermanIKAPlayerController::OnDownReleased);
		InputComponent->BindAction("MoveRightP2", IE_Released, this, &ABombermanIKAPlayerController::OnRightReleased);
	}
}

void ABombermanIKAPlayerController::OnUpPressed()
{
	AccumulatedMovement.X += 1.f;
}

void ABombermanIKAPlayerController::OnUpReleased()
{
	AccumulatedMovement.X -= 1.f;
}

void ABombermanIKAPlayerController::OnLeftPressed()
{
	AccumulatedMovement.Y -= 1.f;
}

void ABombermanIKAPlayerController::OnLeftReleased()
{
	AccumulatedMovement.Y += 1.f;
}

void ABombermanIKAPlayerController::OnDownPressed()
{
	AccumulatedMovement.X -= 1.f;
}

void ABombermanIKAPlayerController::OnDownReleased()
{
	AccumulatedMovement.X += 1.f;
}

void ABombermanIKAPlayerController::OnRightPressed()
{
	AccumulatedMovement.Y += 1.f;
}

void ABombermanIKAPlayerController::OnRightReleased()
{
	AccumulatedMovement.Y -= 1.f;
}





