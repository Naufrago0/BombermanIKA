#include "BIKGameViewportClient.h"
#include "BombermanIKAGameMode.h"
#include "Engine.h"

bool UBIKGameViewportClient::InputKey(FViewport* Viewport, int32 ControllerId, FKey Key, EInputEvent EventType, float AmountDepressed, bool bGamepad)
{
	if (IgnoreInput() || bGamepad || Key.IsMouseButton())
	{
		return Super::InputKey(Viewport, ControllerId, Key, EventType, AmountDepressed, bGamepad);
	}
	else
	{
		// Propagate keyboard events to all players

		UEngine* Engine = GetOuterUEngine();
		int32 NumPlayers = Engine != nullptr ? Engine->GetNumGamePlayers(this) : 0;
		bool bResult = false;

		for (int32 i = 0; i < NumPlayers; ++i)
		{
			bool InputKeyResult = Super::InputKey(Viewport, i, Key, EventType, AmountDepressed, bGamepad);
			bResult =  InputKeyResult || bResult;
		}

		return bResult;
	}
}

void UBIKGameViewportClient::LostFocus(FViewport* Viewport)
{
	UGameInstance* GameInstance = GetGameInstance();
	UWorld* World = nullptr;
	ABombermanIKAGameMode* GameMode = nullptr;
	if (GameInstance != nullptr)
	{
		World = GameInstance->GetWorld();
	}

	if (World != nullptr)
	{
		GameMode = Cast<ABombermanIKAGameMode>(World->GetAuthGameMode());
	}

	if (GameMode != nullptr)
	{
		GameMode->OnApplicationLostFocus();
	}

	Super::LostFocus(Viewport);
}

void UBIKGameViewportClient::ReceivedFocus(FViewport* Viewport)
{
	Super::ReceivedFocus(Viewport);

	UGameInstance* GameInstance = GetGameInstance();
	UWorld* World = nullptr;
	ABombermanIKAGameMode* GameMode = nullptr;
	if (GameInstance != nullptr)
	{
		World = GameInstance->GetWorld();
	}

	if (World != nullptr)
	{
		GameMode = Cast<ABombermanIKAGameMode>(World->GetAuthGameMode());
	}

	if (GameMode != nullptr)
	{
		GameMode->OnApplicationReceivedFocus();
	}
}
