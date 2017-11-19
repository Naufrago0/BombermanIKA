// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "BombermanIKAPlayerController.h"
#include "BombermanIKACharacter.h"
#include "BombermanIKAGameMode.h"
#include "BIKLevelBlock.h"
#include "BIKPowerUpActor.h"
#include "GameFramework/CharacterMovementComponent.h"

const int32 ABombermanIKAPlayerController::MaxBombs = 5;
const int32 ABombermanIKAPlayerController::MaxBombBlockRadius = 5;
const float ABombermanIKAPlayerController::NormalWalkSpeed = 400.f;
const float ABombermanIKAPlayerController::PowerUpWalkSpeed = 600.f;
const float ABombermanIKAPlayerController::RemoteControlDurationSeconds = 15.f;

ABombermanIKAPlayerController::ABombermanIKAPlayerController()
{
	bShowMouseCursor = false;
	bIsFirstPlayer = true;
	bIsAlive = true;
	BombsUpperLimit = 1;
	Bombs = BombsUpperLimit;
	BombBlockRadius = 1;
	RemoteControlSeconds = 0.f;
}

void ABombermanIKAPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	// keep updating the destination every tick while desired
	if (AccumulatedMovement != FVector::ZeroVector)
	{
		ACharacter* Character = GetCharacter();
		if (Character != nullptr)
			Character->AddMovementInput(AccumulatedMovement);
	}

	auto GameMode = Cast<ABombermanIKAGameMode>(GetWorld()->GetAuthGameMode());
	check(GameMode != nullptr);
	ACharacter* Character = GetCharacter();
	check(Character != nullptr);
	FBIKLevelBlock* LevelBlock = GameMode->GetBlockFromLocation(Character->GetActorLocation());
	if (LevelBlock->IsPowerUp())
	{
		LevelBlock->PowerUp->TakePowerUp(this);
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
		if(GameMode->SpawnBombForPlayer(this))
			--Bombs; // Only discount bomb if it was successfully planted.
	}
}

void ABombermanIKAPlayerController::OnBombExploded()
{
	// Return one bomb to the bag of bombs
	++Bombs;
	Bombs = FMath::Clamp(Bombs, 0, BombsUpperLimit);
}

void ABombermanIKAPlayerController::ApplyPowerUp(EPowerUpType PowerUpType)
{
	ACharacter* Character = nullptr;

	switch (PowerUpType)
	{
		case EPowerUpType::ExtraBomb:
			//Increase current account of bombd and bomb limit
			if (BombsUpperLimit < MaxBombs)
			{
				++Bombs;
				++BombsUpperLimit;
			}
			break;
		case EPowerUpType::ExtraExplosion:
			if (BombBlockRadius < MaxBombBlockRadius)
			{
				++BombBlockRadius;
			}
			break;
		case EPowerUpType::SpeedUp:
			Character = GetCharacter();
			if (Character != nullptr)
			{
				auto MovComp = Character->GetCharacterMovement();
				if (MovComp != nullptr)
				{
					MovComp->MaxWalkSpeed = PowerUpWalkSpeed;
				}
			}
			break;
		case EPowerUpType::RemoteControl:
			RemoteControlSeconds = RemoteControlDurationSeconds;
			break;
		default:
			break;
	}
}


