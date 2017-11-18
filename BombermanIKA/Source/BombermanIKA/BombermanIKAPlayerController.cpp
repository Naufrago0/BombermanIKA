// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "BombermanIKAPlayerController.h"
#include "AI/Navigation/NavigationSystem.h"
#include "Runtime/Engine/Classes/Components/DecalComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "BombermanIKACharacter.h"
#include "BombermanIKAGameMode.h"
#include "BIKLevelBlock.h"

const int32 ABombermanIKAPlayerController::MaxBombs = 5;

ABombermanIKAPlayerController::ABombermanIKAPlayerController()
{
	bShowMouseCursor = false;
	bIsFirstPlayer = true;
	bIsAlive = true;
	BombsUpperLimit = 1;
	Bombs = BombsUpperLimit;
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

		InputComponent->BindAction("FireBombP1", IE_Released, this, &ABombermanIKAPlayerController::OnBombFired);
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

		InputComponent->BindAction("FireBombP2", IE_Released, this, &ABombermanIKAPlayerController::OnBombFired);
	}
}

void ABombermanIKAPlayerController::Possess(class APawn* aPawn)
{
	ABombermanIKACharacter* Character = Cast<ABombermanIKACharacter>(aPawn);
	if (Character != nullptr && !bIsFirstPlayer)
	{
		Character->ConfigureP2Character();
	}

	Super::Possess(aPawn);
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

void ABombermanIKAPlayerController::OnBombFired()
{
	if (Bombs <= 0)
		return;

	// This is done in the GameMode for comfort, if we had a networked game we must move the SpawnBombForPlayer logic to the GameState
	auto GameMode = Cast<ABombermanIKAGameMode>(GetWorld()->GetAuthGameMode());
	if (GameMode != nullptr)
	{
		GameMode->SpawnBombForPlayer(this);
		--Bombs;
	}
}

void ABombermanIKAPlayerController::OnBombExploded()
{
	// Return one bomb to the bag of bombs
	++Bombs;
	FMath::Clamp(Bombs, 0, BombsUpperLimit);
}




