#pragma once

#include "Engine/GameViewportClient.h"
#include "BIKGameViewportClient.generated.h"

/**
*
*/
UCLASS()
class UBIKGameViewportClient : public UGameViewportClient
{
	GENERATED_BODY()

public:
	/** 
	 * Overriden InputKey method to pass keyboard input to all local player controllers and allow "couch" multiplayer to work
	 */
	virtual bool InputKey(FViewport* Viewport, int32 ControllerId, FKey Key, EInputEvent EventType, float AmountDepressed = 1.f, bool bGamepad = false) override;
};

