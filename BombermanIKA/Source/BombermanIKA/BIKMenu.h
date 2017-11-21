#pragma once

#include "UserWidget.h"
#include "BIKMenu.generated.h"

UCLASS()
class UBIKMenu : public UUserWidget
{
	GENERATED_BODY()

public:

	/** Initalized the Bomberman menu*/
	UFUNCTION(BlueprintImplementableEvent)
	void InitializeMenu();

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateScore(const int32 PlayerIndex, const int32 Score);

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateBombs(const int32 PlayerIndex, const int32 CurrentBombs, const int32 TotalBombs);

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateBlast(const int32 PlayerIndex, const FString& Blast);

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateRemoteControlTime(const int32 PlayerIndex, const float RemoteControlTime);

	UFUNCTION(BlueprintImplementableEvent)
	void ShowWinner(const int32 PlayerIndex);

	UFUNCTION(BlueprintImplementableEvent)
	void ShowDraw();

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateRoundTime(const float RoundTime);
};
