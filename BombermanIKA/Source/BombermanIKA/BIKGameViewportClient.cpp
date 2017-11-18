#include "BIKGameViewportClient.h"
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
