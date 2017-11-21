#pragma once

#include "GameFramework/Actor.h"
#include "BIKPanCameraController.generated.h"

UCLASS()
class ABIKPanCameraController : public AActor
{
	GENERATED_BODY()

public:
	ABIKPanCameraController();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

protected:

	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class ABombermanIKACharacter* Player1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class ABombermanIKACharacter* Player2;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class ABombermanIKAPlayerController* PC1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class ABombermanIKAPlayerController* PC2;
};
