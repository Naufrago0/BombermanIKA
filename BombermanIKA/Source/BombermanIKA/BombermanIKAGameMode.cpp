// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "BombermanIKAGameMode.h"
#include "BombermanIKA.h"
#include "BombermanIKAPlayerController.h"
#include "BombermanIKACharacter.h"
#include "BIKBombActor.h"
#include "BIKExplosionActor.h"
#include "BIKPowerUpActor.h"
#include "BIKDestructibleBlockActor.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/LocalPlayer.h"


ABombermanIKAGameMode::ABombermanIKAGameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = ABombermanIKAPlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/TopDownCPP/Blueprints/TopDownCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

void ABombermanIKAGameMode::BeginPlay()
{
	check(GetGameInstance() != nullptr);
	check(GetGameInstance()->GetGameViewportClient() != nullptr);

	GetGameInstance()->GetGameViewportClient()->SetDisableSplitscreenOverride(true);

	GenerateLevel();

	// Create second player
	FString ErrorMsg;
	ULocalPlayer* Player2 = GetGameInstance()->CreateLocalPlayer(1, ErrorMsg, true);
	if (Player2 != nullptr)
	{
		Player2->GetPlayerController(GetWorld());
	}

	if (!ErrorMsg.IsEmpty())
		UE_LOG(LogBombermanIKA, Error, TEXT("ERROR: %s"), *ErrorMsg);
}

void ABombermanIKAGameMode::GenerateLevel()
{
	DestructibleBlocks = FMath::Clamp(DestructibleBlocks, 0, 90);

	for (int32 i = 0; i < LEVEL_WIDTH; ++i)
	{
		for (int32 j = 0; j < LEVEL_HEIGHT; ++j)
		{
			if ((i & 1) && (j & 1)) //Create indestructible blocks in all odd coordinates blocks
			{
				FVector Location(j * -100.f - 50.f, i * 100.f + 50.f, 100.f);
				LevelBlocksLogic[i][j].IndestructibleBlock = GetWorld()->SpawnActor(BlockActorClass, &Location);
			}
		}
	}

	int32 RemainingDestructibleBlocks = DestructibleBlocks;
	while (RemainingDestructibleBlocks > 0)
	{
		int32 Col = FMath::RandRange(0, LEVEL_WIDTH - 1);
		int32 Row = FMath::RandRange(0, LEVEL_HEIGHT - 1);

		//Check forbidden blocks
		bool P1Start = (Col == 0) && (Row == 0);
		bool P1StartRight = (Col == 1) && (Row == 0);
		bool P1StartDown = (Col == 0) && (Row == 1);
		bool P2Start = (Col == (LEVEL_WIDTH - 1)) && (Row == (LEVEL_HEIGHT - 1));
		bool P2StartLeft = (Col == (LEVEL_WIDTH - 2)) && (Row == (LEVEL_HEIGHT - 1));
		bool P2StartUp = (Col == (LEVEL_WIDTH - 1)) && (Row == (LEVEL_HEIGHT - 2));
		
		if (P1Start || P1StartDown || P1StartRight || P2Start || P2StartLeft || P2StartUp)
			continue;

		FBIKLevelBlock& SelectedBlock = LevelBlocksLogic[Col][Row];
		if (!SelectedBlock.IsDestroyable() && !SelectedBlock.IsIndestructible())
		{
			FVector BlockLocation = FBIKLevelBlock::GetBlockCenterFromRowColIndexes(Row, Col);
			FTransform Transform(BlockLocation);
			SelectedBlock.DestroyableBlock = GetWorld()->SpawnActor<ABIKDestructibleBlockActor>(DestructibleBlockActorClass, Transform);
			SelectedBlock.DestroyableBlock->SetLevelBlock(&SelectedBlock);
			--RemainingDestructibleBlocks;
		}
	}
}

FString ABombermanIKAGameMode::InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal)
{
	ABombermanIKAPlayerController* BIKPC = Cast<ABombermanIKAPlayerController>(NewPlayerController);
	check(BIKPC != nullptr);

	//Determine using Portal if it's first or second player
	BIKPC->SetFirstPlayer(Portal == TEXT("P1"));

	return Super::InitNewPlayer(NewPlayerController, UniqueId, Options, Portal);
}

APlayerController* ABombermanIKAGameMode::Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal, const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(NewPlayer);
	check(LocalPlayer != nullptr);

	FString NewPortal(Portal);

	if (NewPortal.IsEmpty())
	{
		if (LocalPlayer->GetControllerId() == 0)
		{
			NewPortal = TEXT("P1");
		}
		else
		{
			NewPortal = TEXT("P2");
		}
	}
	
	return Super::Login(NewPlayer, InRemoteRole, NewPortal, Options, UniqueId, ErrorMessage);
}

bool ABombermanIKAGameMode::SpawnBombForPlayer(ABombermanIKAPlayerController* PC)
{
	check(PC != nullptr);

	auto Character = PC->GetCharacter();
	if (Character != nullptr)
	{
		FVector BombLocation = FBIKLevelBlock::GetBlockCenterFromCurrentLocation(Character->GetActorLocation());
		FBIKLevelBlock* LevelBlock = GetBlockFromLocation(BombLocation);

		// We can only spawn one bomb in a given level block
		if (!LevelBlock->IsBombSpawned())
		{
			FActorSpawnParameters SpawnParameters;
			SpawnParameters.Instigator = Character;
			FTransform Transform(BombLocation);
			ABIKBombActor* Bomb = GetWorld()->SpawnActor<ABIKBombActor>(BombActorClass, Transform, SpawnParameters);
			Bomb->ConfigureBomb(LevelBlock, PC->GetBombBlockRadius());
			return true;
		}
	}

	return false;
}

FBIKLevelBlock* ABombermanIKAGameMode::GetBlockFromLocation(const FVector& Position)
{
	int32 Column = INDEX_NONE; 
	int32 Row = INDEX_NONE; 

	GetBlockIndexesFromLocation(Position, Row, Column);

	return &LevelBlocksLogic[Column][Row];
}

void ABombermanIKAGameMode::GetBlockIndexesFromLocation(const FVector& Position, int32& Row, int32& Column)
{
	Column = FMath::Clamp(FMath::TruncToInt(FMath::Abs(Position.Y) / FBIKLevelBlock::BLOCK_SIZE), 0, LEVEL_WIDTH);
	Row = FMath::Clamp(FMath::TruncToInt(FMath::Abs(Position.X) / FBIKLevelBlock::BLOCK_SIZE), 0, LEVEL_HEIGHT);
}

void ABombermanIKAGameMode::ExplodeBomb(ABIKBombActor* ExplodedBomb)
{
	check(ExplodedBomb != nullptr);
	int32 Radius = ExplodedBomb->GetBombBlockRadius();

	struct FFoundLevelBlock
	{
		FBIKLevelBlock* LevelBlock;
		int32 Row;
		int32 Col;
		int32 DistanceFromBomb;

		FFoundLevelBlock(FBIKLevelBlock* LevelBlockArg, const int32 RowArg, const int32 ColArg, const int32 DistanceFromBombArg)
			: LevelBlock(LevelBlockArg)
			, Row(RowArg)
			, Col(ColArg)
			, DistanceFromBomb(DistanceFromBombArg)
		{
		}
	};

	TArray<FFoundLevelBlock> ExplodedLevelBlocks;
	FVector BombLocation = ExplodedBomb->GetActorLocation();
	int32 InitialRow;
	int32 InitialCol;
	GetBlockIndexesFromLocation(BombLocation, InitialRow, InitialCol);

	//Add the level block where the bomb is standing to the list of exploded blocks
	FBIKLevelBlock* BombBlock = GetBlockFromLocation(BombLocation);
	// Remove the bomb from its block so we don't explode it again
	BombBlock->Bomb = nullptr;
	ExplodedLevelBlocks.Add(FFoundLevelBlock(BombBlock, InitialRow, InitialCol, 0));

	//This array specifies how to increase row and column indexes to check the four directions
	struct FDirInc
	{
		int32 RowInc;
		int32 ColInc;
	};

	//                                         UP      LEFT     DOWN    RIGHT
	static const FDirInc RowColIndexesInc[]{ {-1, 0}, {0, -1}, {1, 0}, {0, 1} };

	int32 CurrentRow;
	int32 CurrentCol;
	for (int32 i = 0; i < 4; ++i)
	{
		for (int32 CurrentRadius = 1; CurrentRadius <= Radius; ++CurrentRadius)
		{
			CurrentRow = InitialRow + RowColIndexesInc[i].RowInc * CurrentRadius;
			CurrentCol = InitialCol + RowColIndexesInc[i].ColInc * CurrentRadius;

			//Check if we are looking outside the map
			if (CurrentRow < 0 || CurrentRow >= LEVEL_HEIGHT || CurrentCol < 0 || CurrentCol >= LEVEL_WIDTH)
				break;

			FBIKLevelBlock& CheckedBlock = LevelBlocksLogic[CurrentCol][CurrentRow];
			// If we find an undestructible block, stop exploding in this direction
			if (CheckedBlock.IsIndestructible())
				break;

			// Calculate distance from bomb to exploded level block in blocks
			int32 DistanceFromBomb = FMath::Max(FMath::Abs(CurrentRow - InitialRow), FMath::Abs(CurrentCol - InitialCol));

			// If we find a destructible block, stop exploding in this direction, but explode the found block
			if (CheckedBlock.IsDestroyable())
			{
				ExplodedLevelBlocks.Add(FFoundLevelBlock(&CheckedBlock, CurrentRow, CurrentCol, DistanceFromBomb));
				break;
			}

			// In any other case add the level block to be exploded and continue checking blocks in this direction
			ExplodedLevelBlocks.Add(FFoundLevelBlock(&CheckedBlock, CurrentRow, CurrentCol, DistanceFromBomb));
		}
	}

	// Check all the marked as exploded blocks to take the right action
	for (auto It = ExplodedLevelBlocks.CreateIterator(); It; ++It)
	{
		// If the block is already exploding, just reset the time of explosion
		if (It->LevelBlock->IsOnExplosion())
		{
			It->LevelBlock->Explosion->SetExplosionSeconds(DefaultExplosionSeconds);
		}
		// Create a new explosion actor in the block
		else
		{
			FVector ExplosionLocation = FBIKLevelBlock::GetBlockCenterFromRowColIndexes(It->Row, It->Col);
			FTransform Transform(ExplosionLocation);
			It->LevelBlock->Explosion = GetWorld()->SpawnActor<ABIKExplosionActor>(ExplosionActorClass, Transform);
			It->LevelBlock->Explosion->ConfigureExplosion(It->LevelBlock, DefaultDelaySecondsPerBlock * It->DistanceFromBomb, DefaultExplosionSeconds);
		}
	}
}

void ABombermanIKAGameMode::SpawnRandomPowerUp(const FVector& Location)
{
	float SpawnPowerUpDiceThrow = FMath::RandRange(0.f, 1.f);
	if (SpawnPowerUpDiceThrow > 0.f && SpawnPowerUpDiceThrow <= SpawnPowerUpProbability)
	{
		FVector PowerUpLocation = FBIKLevelBlock::GetBlockCenterFromCurrentLocation(Location);
		FBIKLevelBlock* LevelBlock = GetBlockFromLocation(PowerUpLocation);

		UClass* PowerUpClasses[]{ ExtraBombPowerUpActorClass, ExplosionRadiusPowerUpActorClass, RemoteControlPowerUpActorClass, SpeedUpPowerUpActorClass };
		EPowerUpType PowerUpTypes[]{ EPowerUpType::ExtraBomb, EPowerUpType::ExtraExplosion, EPowerUpType::RemoteControl, EPowerUpType::SpeedUp };

		int32 PowerUpSelected = FMath::RandRange(0, 3);

		FTransform Transform(PowerUpLocation);
		LevelBlock->PowerUp = GetWorld()->SpawnActor<ABIKPowerUpActor>(PowerUpClasses[PowerUpSelected], Transform);
		LevelBlock->PowerUp->ConfigurePowerUp(LevelBlock, PowerUpTypes[PowerUpSelected]);
	}
}