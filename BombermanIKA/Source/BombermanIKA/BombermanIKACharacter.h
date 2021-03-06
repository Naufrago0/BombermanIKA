// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BombermanIKACharacter.generated.h"

UCLASS(Blueprintable)
class ABombermanIKACharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ABombermanIKACharacter();

	// Called every frame.
	virtual void Tick(float DeltaSeconds) override;

	/** Set material for player 2 if this is the player 2's character */
	void ConfigureP2Character();

	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

private:
	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Material used for second player */
	UPROPERTY(EditAnywhere, Category = "BombermanIKACharacter")
	class UMaterial* P2Material;
};

