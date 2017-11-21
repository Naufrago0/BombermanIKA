// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "BombermanIKAGameMode.h"
#include "BombermanIKA.h"
#include "BombermanIKAPlayerController.h"
#include "BombermanIKACharacter.h"
#include "BIKBombActor.h"
#include "BIKExplosionActor.h"
#include "BIKPowerUpActor.h"
#include "BIKPlayerState.h"
#include "BIKDestructibleBlockActor.h"
#include "BIKMenu.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/LocalPlayer.h"


ABombermanIKAGameMode::ABombermanIKAGameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = ABombermanIKAPlayerController::StaticClass();
	PlayerStateClass = ABIKPlayerState::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/TopDownCPP/Blueprints/TopDownCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
	RoundTime = 0.f;
	PrimaryActorTick.bCanEverTick = true;
	bSetFocusNextTick = false;
}

void ABombermanIKAGameMode::BeginPlay()
{
	Super::BeginPlay();

#if !WITH_EDITOR
	GetWorld()->Exec(GetWorld(), TEXT("r.setres 1280x720w"));
#endif

	check(GetGameInstance() != nullptr);
	check(GetGameInstance()->GetGameViewportClient() != nullptr);

	GetGameInstance()->GetGameViewportClient()->SetDisableSplitscreenOverride(true);

	// Create indestructible blocks
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

	GenerateBlockLevel();

	// Init round time
	RoundTime = RoundDurationSeconds;

	// Create second player
	FString ErrorMsg;
	ULocalPlayer* Player1 = GetGameInstance()->GetLocalPlayerByIndex(0);
	check(Player1 != nullptr);
	
	ULocalPlayer* Player2 = GetGameInstance()->CreateLocalPlayer(1, ErrorMsg, true);
	check(Player2 != nullptr);

	if (!ErrorMsg.IsEmpty())
		UE_LOG(LogBombermanIKA, Error, TEXT("ERROR: %s"), *ErrorMsg);

	//Initialize UI
	InGameMenu = CreateWidget<UBIKMenu>(GetWorld(), InGameMenuWidgetClass);
	check(InGameMenu != nullptr);
	InGameMenu->InitializeMenu();
	InGameMenu->AddToViewport();
	InGameMenu->SetKeyboardFocus();
	InGameMenu->SetUserFocus(Player1->GetPlayerController(GetWorld()));

	// Pause game until is started from UI
	Player1->GetPlayerController(GetWorld())->SetPause(true);

	// Show initial values in UI
	ABombermanIKAPlayerController* PC1 = Cast<ABombermanIKAPlayerController>(Player1->GetPlayerController(GetWorld()));
	PC1->UpdateHUDBombs();
	PC1->UpdateHUDBlast();
	PC1->UpdateHUDScore();
	PC1->UpdateHUDRemoteControlTime();

	ABombermanIKAPlayerController* PC2 = Cast<ABombermanIKAPlayerController>(Player2->GetPlayerController(GetWorld()));
	PC2->UpdateHUDBombs();
	PC2->UpdateHUDBlast();
	PC2->UpdateHUDScore();
	PC2->UpdateHUDRemoteControlTime();
}

void ABombermanIKAGameMode::GenerateBlockLevel()
{
	DestructibleBlocks = FMath::Clamp(DestructibleBlocks, 0, 90);

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

ABIKBombActor* ABombermanIKAGameMode::SpawnBombForPlayer(ABombermanIKAPlayerController* PC)
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
			Bomb->ConfigureBomb(LevelBlock, PC->GetBombBlockRadius(), PC->IsRemoteControlling() ? 0.f : TimeToExplosionSeconds);
			return Bomb;
		}
	}

	return nullptr;
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

void ABombermanIKAGameMode::UpdateHUDScore(const int32 PlayerIndex, const int32 Score)
{
	InGameMenu->UpdateScore(PlayerIndex, Score);
}

void ABombermanIKAGameMode::UpdateHUDBombs(const int32 PlayerIndex, const int32 CurrentBombs, const int32 TotalBombs)
{
	InGameMenu->UpdateBombs(PlayerIndex, CurrentBombs, TotalBombs);
}

void ABombermanIKAGameMode::UpdateHUDBlast(const int32 PlayerIndex, const FString& Blast)
{
	InGameMenu->UpdateBlast(PlayerIndex, Blast);
}

void ABombermanIKAGameMode::UpdateHUDRemoteControlTime(const int32 PlayerIndex, const float RemoteControlTime)
{
	InGameMenu->UpdateRemoteControlTime(PlayerIndex, RemoteControlTime);
}

void ABombermanIKAGameMode::PlayerDied(ABombermanIKAPlayerController* PC)
{
	UE_LOG(LogBombermanIKA, Display, TEXT("Player: [%d] died"), PC->IsFirstPlayer() ? 0 : 1);
	if (Loser == nullptr)
	{
		Loser = PC;
		Loser->DisableInput(nullptr);
		DyingTime = 1.5f;
	}
	else
		Loser = nullptr; // Both players died
}

void ABombermanIKAGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (DyingTime > 0.f)
	{
		DyingTime -= DeltaTime;
		if (DyingTime <= 0.f)
		{
			EndRound();
		}
	}
	else if (RoundTime > 0.f)
	{
		RoundTime -= DeltaTime;
		if (RoundTime <= 0.f)
		{
			EndRound();
		}
	}

	InGameMenu->UpdateRoundTime(RoundTime);

	if (bSetFocusNextTick)
	{
		UGameInstance* GameInstance = GetGameInstance();
		ULocalPlayer* LocalPlayer = nullptr;
		APlayerController* PC = nullptr;

		if (InGameMenu != nullptr && !InGameMenu->HasKeyboardFocus())
		{
			InGameMenu->SetKeyboardFocus();

			if (GameInstance != nullptr)
			{
				LocalPlayer = GameInstance->GetLocalPlayerByIndex(0);
			}

			if (LocalPlayer != nullptr)
			{
				PC = LocalPlayer->GetPlayerController(GetWorld());
			}

			if (PC != nullptr)
			{
				InGameMenu->SetUserFocus(PC);
				PC->SetPause(true);
			}
		}
		bSetFocusNextTick = false;
	}
}

void ABombermanIKAGameMode::EndRound()
{
	ULocalPlayer* Player1 = GetGameInstance()->GetLocalPlayerByIndex(0);
	check(Player1 != nullptr);
	auto PC1 = Cast<ABombermanIKAPlayerController>(Player1->GetPlayerController(GetWorld()));
	check(PC1 != nullptr);
	ULocalPlayer* Player2 = GetGameInstance()->GetLocalPlayerByIndex(1);
	check(Player2 != nullptr);
	auto PC2 = Cast<ABombermanIKAPlayerController>(Player2->GetPlayerController(GetWorld()));
	check(PC2 != nullptr);

	auto FirstPC = GetGameInstance()->GetFirstLocalPlayerController(GetWorld());
	if (Loser != nullptr)
		InGameMenu->ShowWinner(Loser->IsFirstPlayer() ? 1 : 0);
	else
		InGameMenu->ShowDraw();

	if (PC1 != Loser)
	{
		Cast<ABIKPlayerState>(PC1->GetCharacter()->PlayerState)->IncreaseScore();
	}
	else
	{
		Cast<ABIKPlayerState>(PC2->GetCharacter()->PlayerState)->IncreaseScore();
	}

	// Reset loser
	Loser = nullptr;

	// Reset round time
	RoundTime = RoundDurationSeconds;

	PC1->ResetPC();
	PC2->ResetPC();
	ResetBlockLevel();

	PC1->SetPause(true);
}

void ABombermanIKAGameMode::ResetBlockLevel()
{
	// Create indestructible blocks
	for (int32 i = 0; i < LEVEL_WIDTH; ++i)
	{
		for (int32 j = 0; j < LEVEL_HEIGHT; ++j)
		{
			FBIKLevelBlock& Block = LevelBlocksLogic[i][j];
			if (Block.IsBombSpawned())
			{
				Block.Bomb->Destroy();
				Block.Bomb = nullptr;
			}
			if (Block.IsDestroyable())
			{
				Block.DestroyableBlock->Destroy();
				Block.DestroyableBlock = nullptr;
			}
			if (Block.IsOnExplosion())
			{
				Block.Explosion->Destroy();
				Block.Explosion = nullptr;
			}
			if (Block.IsPowerUp())
			{
				Block.PowerUp->Destroy();
				Block.PowerUp = nullptr;
			}
		}
	}

	GenerateBlockLevel();
}

void ABombermanIKAGameMode::OnApplicationLostFocus()
{
	UGameInstance* GameInstance = GetGameInstance();
	ULocalPlayer* LocalPlayer = nullptr;
	APlayerController* PC = nullptr;

	if (InGameMenu != nullptr)
	{
		InGameMenu->InitializeMenu();

		if (GameInstance != nullptr)
		{
			LocalPlayer = GameInstance->GetLocalPlayerByIndex(0);
		}

		if (LocalPlayer != nullptr)
		{
			PC = LocalPlayer->GetPlayerController(GetWorld());
		}

		if(PC != nullptr)
			PC->SetPause(true);
	}
}

void ABombermanIKAGameMode::OnApplicationReceivedFocus()
{
	if (!bSetFocusNextTick)
		bSetFocusNextTick = true;

	UGameInstance* GameInstance = GetGameInstance();
	ULocalPlayer* LocalPlayer = nullptr;
	APlayerController* PC = nullptr;

	if (GameInstance != nullptr)
	{
		LocalPlayer = GameInstance->GetLocalPlayerByIndex(0);
	}

	if (LocalPlayer != nullptr)
	{
		PC = LocalPlayer->GetPlayerController(GetWorld());
	}

	if (PC != nullptr)
		PC->SetPause(false);
}