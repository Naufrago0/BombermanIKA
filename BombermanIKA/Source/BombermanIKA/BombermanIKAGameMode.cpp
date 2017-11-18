// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "BombermanIKAGameMode.h"
#include "BombermanIKA.h"
#include "BombermanIKAPlayerController.h"
#include "BombermanIKACharacter.h"
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
	for (int32 i = 0; i < LEVEL_WIDTH; ++i)
	{
		for (int32 j = 0; j < LEVEL_HEIGHT; ++j)
		{
			if ((i & 1) && (j & 1))
			{
				FVector Location(j * -100.f - 50.f, i * 100.f + 50.f, 100.f);
				LevelBlocksLogic[i][j].Block = GetWorld()->SpawnActor(BlockActorClass, &Location);
			}
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
