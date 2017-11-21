// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "BombermanIKAPlayerController.h"
#include "BombermanIKA.h"
#include "BombermanIKACharacter.h"
#include "BombermanIKAGameMode.h"
#include "BIKLevelBlock.h"
#include "BIKPowerUpActor.h"
#include "BIKBombActor.h"
#include "BIKPlayerState.h"
#include "GameFramework/CharacterMovementComponent.h"

const int32 ABombermanIKAPlayerController::MaxBombs = 5;
const int32 ABombermanIKAPlayerController::MaxBombBlockRadius = 5;
const float ABombermanIKAPlayerController::NormalWalkSpeed = 400.f;
const float ABombermanIKAPlayerController::PowerUpWalkSpeed = 600.f;

ABombermanIKAPlayerController::ABombermanIKAPlayerController()
{
	bShowMouseCursor = false;
	bIsFirstPlayer = true;
	bIsAlive = true;
	BombsUpperLimit = 1;
	Bombs = BombsUpperLimit;
	BombBlockRadius = 1;
	RemoteControlSeconds = 0.f;
	RemoteBomb = nullptr;
	ResetMovement();
}

void ABombermanIKAPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	// Update movement
	ACharacter* Character = GetCharacter();
	if (Character != nullptr)
	{
		// X Axis
		AccumulatedMovement.X = UpPressed != DownPressed ? UpPressed ? 1.f : -1.f : 0.f;
		AccumulatedMovement.Y = LeftPressed != RightPressed ? LeftPressed ? -1.f : 1.f : 0.f;
		AccumulatedMovement.Z = 0.f;
		Character->AddMovementInput(AccumulatedMovement);
	}

	auto GameMode = Cast<ABombermanIKAGameMode>(GetWorld()->GetAuthGameMode());
	if (GameMode != nullptr)
	{
		ACharacter* Character = GetCharacter();
		check(Character != nullptr);
		FBIKLevelBlock* LevelBlock = GameMode->GetBlockFromLocation(Character->GetActorLocation());
		if (LevelBlock->IsPowerUp())
		{
			LevelBlock->PowerUp->TakePowerUp(this);
		}

		if (LevelBlock->IsOnExplosion() && bIsAlive)
		{
			bIsAlive = false;
			GameMode->PlayerDied(this);
		}

		if (RemoteControlSeconds > 0.f)
		{
			RemoteControlSeconds -= DeltaTime;
			UpdateHUDRemoteControlTime();
			if (RemoteControlSeconds <= 0)
			{
				if (RemoteBomb != nullptr)
				{
					RemoteBomb->ExplodeBomb();
					RemoteBomb = nullptr;
				}
			}
		}
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
	UE_LOG(LogBombermanIKA, Verbose, TEXT("OnUpPressed, [%d] [%s]"), bIsFirstPlayer ? 0 : 1, *AccumulatedMovement.ToString());
	UpPressed = true;
}

void ABombermanIKAPlayerController::OnUpReleased()
{
	UE_LOG(LogBombermanIKA, Verbose, TEXT("OnUpReleased, [%d] [%s]"), bIsFirstPlayer ? 0 : 1, *AccumulatedMovement.ToString());
	UpPressed = false;
}

void ABombermanIKAPlayerController::OnLeftPressed()
{
	UE_LOG(LogBombermanIKA, Verbose, TEXT("OnUpPressed, [%d] [%s]"), bIsFirstPlayer ? 0 : 1, *AccumulatedMovement.ToString());
	LeftPressed = true;
}

void ABombermanIKAPlayerController::OnLeftReleased()
{
	UE_LOG(LogBombermanIKA, Verbose, TEXT("OnLeftReleased, [%d] [%s]"), bIsFirstPlayer ? 0 : 1, *AccumulatedMovement.ToString());
	LeftPressed = false;
}

void ABombermanIKAPlayerController::OnDownPressed()
{
	UE_LOG(LogBombermanIKA, Verbose, TEXT("OnDownPressed, [%d] [%s]"), bIsFirstPlayer ? 0 : 1, *AccumulatedMovement.ToString());
	DownPressed = true;
}

void ABombermanIKAPlayerController::OnDownReleased()
{
	UE_LOG(LogBombermanIKA, Verbose, TEXT("OnDownReleased, [%d] [%s]"), bIsFirstPlayer ? 0 : 1, *AccumulatedMovement.ToString());
	DownPressed = false;
}

void ABombermanIKAPlayerController::OnRightPressed()
{
	UE_LOG(LogBombermanIKA, Verbose, TEXT("OnRightPressed, [%d] [%s]"), bIsFirstPlayer ? 0 : 1, *AccumulatedMovement.ToString());
	RightPressed = true;
}

void ABombermanIKAPlayerController::OnRightReleased()
{
	UE_LOG(LogBombermanIKA, Verbose, TEXT("OnRightReleased, [%d] [%s]"), bIsFirstPlayer ? 0 : 1, *AccumulatedMovement.ToString());
	RightPressed = false;
}

void ABombermanIKAPlayerController::OnBombFired()
{
	bool ShouldSpawnBomb = IsRemoteControlling() && RemoteBomb == nullptr || !IsRemoteControlling();
	bool ShouldExplodeBomb = IsRemoteControlling() && RemoteBomb != nullptr;

	if (Bombs <= 0 && ShouldSpawnBomb)
		return;

	// This is done in the GameMode for comfort, if we had a networked game we must move the SpawnBombForPlayer logic to the GameState
	auto GameMode = Cast<ABombermanIKAGameMode>(GetWorld()->GetAuthGameMode());
	if (GameMode != nullptr)
	{
		UE_LOG(LogBombermanIKA, Display, TEXT("Player [%d], RemoteBomb: [%s], IsRemoteControlling: [%s]"), IsFirstPlayer() ? 0 : 1, RemoteBomb != nullptr ? *RemoteBomb->GetName() : TEXT("NULL"), IsRemoteControlling() ? TEXT("TRUE") : TEXT("FALSE"));

		if (ShouldSpawnBomb)
		{
			auto NewBomb = GameMode->SpawnBombForPlayer(this);
			if (NewBomb != nullptr)
			{
				// Store the bomb if we are remote controlling
				if (IsRemoteControlling())
				{
					RemoteBomb = NewBomb;
				}

				--Bombs; // Only discount bomb if it was successfully planted.
				//Update UI
				UpdateHUDBombs();
			}
		}
		else if (ShouldExplodeBomb)
		{
			RemoteBomb->ExplodeBomb();
			RemoteBomb = nullptr;
		}
	}
}

void ABombermanIKAPlayerController::OnBombExploded()
{
	// Return one bomb to the bag of bombs
	++Bombs;
	Bombs = FMath::Clamp(Bombs, 0, BombsUpperLimit);
	UpdateHUDBombs();

}

void ABombermanIKAPlayerController::ApplyPowerUp(EPowerUpType PowerUpType)
{
	ACharacter* Character = nullptr;
	auto GameMode = Cast<ABombermanIKAGameMode>(GetWorld()->GetAuthGameMode());

	switch (PowerUpType)
	{
		case EPowerUpType::ExtraBomb:
			//Increase current account of bombd and bomb limit
			if (BombsUpperLimit < MaxBombs)
			{
				++Bombs;
				++BombsUpperLimit;
				UpdateHUDBombs();
			}
			break;
		case EPowerUpType::ExtraExplosion:
			if (BombBlockRadius < MaxBombBlockRadius)
			{
				++BombBlockRadius;
				UpdateHUDBlast();
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
			if (GameMode != nullptr)
			{
				RemoteControlSeconds = GameMode->RemoteControlDurationSeconds;
				UpdateHUDRemoteControlTime();
			}
			break;
		default:
			break;
	}
}

void ABombermanIKAPlayerController::UpdateHUDScore()
{
	auto GameMode = Cast<ABombermanIKAGameMode>(GetWorld()->GetAuthGameMode());

	if (GameMode != nullptr)
	{
		ABIKPlayerState* BIKPlayerState = Cast<ABIKPlayerState>(GetCharacter()->PlayerState);
		GameMode->UpdateHUDScore(bIsFirstPlayer ? 0 : 1, BIKPlayerState->RoundsScore);
	}
}

void ABombermanIKAPlayerController::UpdateHUDBombs()
{
	auto GameMode = Cast<ABombermanIKAGameMode>(GetWorld()->GetAuthGameMode());
	
	if (GameMode != nullptr)
	{
		GameMode->UpdateHUDBombs(bIsFirstPlayer ? 0 : 1, Bombs, BombsUpperLimit);
	}
}


void ABombermanIKAPlayerController::UpdateHUDBlast()
{
	auto GameMode = Cast<ABombermanIKAGameMode>(GetWorld()->GetAuthGameMode());

	if (GameMode != nullptr)
	{
		FString BlastStr;
		for (int32 i = 0; i < BombBlockRadius; ++i)
			BlastStr += TEXT("X");
		GameMode->UpdateHUDBlast(bIsFirstPlayer ? 0 : 1, BlastStr);
	}
}

void ABombermanIKAPlayerController::UpdateHUDRemoteControlTime()
{
	auto GameMode = Cast<ABombermanIKAGameMode>(GetWorld()->GetAuthGameMode());

	if (GameMode != nullptr)
	{
		GameMode->UpdateHUDRemoteControlTime(bIsFirstPlayer ? 0 : 1, RemoteControlSeconds);
	}
}

void ABombermanIKAPlayerController::ResetMovement()
{
	AccumulatedMovement = FVector::ZeroVector;
	UpPressed = false;
	LeftPressed = false;
	DownPressed = false;
	RightPressed = false;
}

void ABombermanIKAPlayerController::ResetPC()
{
	GetCharacter()->SetActorLocation(FBIKLevelBlock::GetBlockCenterFromRowColIndexes(IsFirstPlayer() ? 0 : LEVEL_HEIGHT - 1, IsFirstPlayer() ? 0 : LEVEL_WIDTH - 1));
	BombsUpperLimit = 1;
	Bombs = BombsUpperLimit;
	bIsAlive = true;
	BombBlockRadius = 1;
	RemoteControlSeconds = 0.f;
	RemoteBomb = nullptr;
	UpdateHUDBlast();
	UpdateHUDBombs();
	UpdateHUDRemoteControlTime();
	UpdateHUDScore();
	EnableInput(nullptr);
	ResetMovement();
}

bool ABombermanIKAPlayerController::IsFirstPlayer() const
{ 
	return bIsFirstPlayer; 
}