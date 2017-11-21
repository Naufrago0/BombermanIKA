#include "BIKPanCameraController.h"
#include "BombermanIKA.h"
#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Character.h"
#include "Camera/CameraComponent.h"
#include "Engine/LocalPlayer.h"
#include "BombermanIKAPlayerController.h"
#include "BombermanIKACharacter.h"

ABIKPanCameraController::ABIKPanCameraController()
{
	//// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->bAbsoluteRotation = true; // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 1000.f;
	CameraBoom->RelativeRotation = FRotator(-60.f, 0.f, 0.f);
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	//// Create a camera...
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	PrimaryActorTick.bCanEverTick = true;
}

void ABIKPanCameraController::BeginPlay()
{
	Super::BeginPlay();
	if (GetGameInstance()->GetNumLocalPlayers() >= 2)
	{
		PC1 = Cast<ABombermanIKAPlayerController>(GetGameInstance()->GetLocalPlayerByIndex(0)->GetPlayerController(GetWorld()));
		PC2 = Cast<ABombermanIKAPlayerController>(GetGameInstance()->GetLocalPlayerByIndex(1)->GetPlayerController(GetWorld()));
		Player1 = Cast<ABombermanIKACharacter>(PC1->GetCharacter());
		Player2 = Cast<ABombermanIKACharacter>(PC2->GetCharacter());
		if (PC1 != nullptr)
		{
			PC1->SetViewTargetWithBlend(this, 0.5);
		}
	}
}

void ABIKPanCameraController::Tick(float DeltaSeconds)
{
	UGameInstance* GameInstance = GetGameInstance();
	if (GameInstance->GetNumLocalPlayers() < 2)
		return;
	else if(PC1 == nullptr || PC2 == nullptr)
	{
		PC1 = Cast<ABombermanIKAPlayerController>(GetGameInstance()->GetLocalPlayerByIndex(0)->GetPlayerController(GetWorld()));
		PC2 = Cast<ABombermanIKAPlayerController>(GetGameInstance()->GetLocalPlayerByIndex(1)->GetPlayerController(GetWorld()));
		Player1 = Cast<ABombermanIKACharacter>(PC1->GetCharacter());
		Player2 = Cast<ABombermanIKACharacter>(PC2->GetCharacter());
		if (PC1 != nullptr)
		{
			PC1->SetViewTargetWithBlend(this, 0.5);
		}
	}

	float UpdateDistance = 5.f;
	float ScreenDistance = 150.f;
	if (Player1 != nullptr && Player2 != nullptr)
	{
		FVector NewLocation = Player1->GetActorLocation() - Player2->GetActorLocation();
		NewLocation *= 0.5;
		NewLocation += Player2->GetActorLocation();
		SetActorLocation(NewLocation);
		UE_LOG(LogBombermanIKA, Verbose, TEXT("Pan Camera Controller Location: [%s]"), *GetActorLocation().ToString());
	}

	FVector2D P1ScreenPos;
	FVector2D P2ScreenPos;
	int32 ScreenSizeXGetter, ScreenSizeYGetter;
	float ScreenSizeX, ScreenSizeY;
	float InnerScreenSizeX, InnerScreenSizeY, OuterScreenSizeX, OuterScreenSizeY, InnerScreenDistance, OuterScreenDistance;
	PC1->ProjectWorldLocationToScreen(Player1->GetActorLocation(), P1ScreenPos);
	PC1->ProjectWorldLocationToScreen(Player2->GetActorLocation(), P2ScreenPos);
	PC1->GetViewportSize(ScreenSizeXGetter, ScreenSizeYGetter);

	InnerScreenDistance = 2 * ScreenDistance;
	OuterScreenDistance = ScreenDistance;
	ScreenSizeX = ScreenSizeXGetter;
	ScreenSizeY = ScreenSizeYGetter;
	InnerScreenSizeX = ScreenSizeX - InnerScreenDistance;
	InnerScreenSizeY = ScreenSizeY - InnerScreenDistance;
	OuterScreenSizeX = ScreenSizeX - OuterScreenDistance;
	OuterScreenSizeY = ScreenSizeY - OuterScreenDistance;

	// Check if we have to move the camera up
	float NewLength = CameraBoom->TargetArmLength;
	if (P1ScreenPos.X <= OuterScreenDistance || P1ScreenPos.X >= OuterScreenSizeX || P2ScreenPos.X <= OuterScreenDistance || P2ScreenPos.X >= OuterScreenSizeX ||
		P1ScreenPos.Y <= OuterScreenDistance || P1ScreenPos.Y >= OuterScreenSizeY || P2ScreenPos.Y <= OuterScreenDistance || P2ScreenPos.Y >= OuterScreenSizeY)
	{
		NewLength = CameraBoom->TargetArmLength + UpdateDistance;
	}
	else if (P1ScreenPos.X >= InnerScreenDistance || P1ScreenPos.X <= InnerScreenSizeX || P2ScreenPos.X >= InnerScreenDistance || P2ScreenPos.X <= InnerScreenSizeX ||
			 P1ScreenPos.Y >= InnerScreenDistance || P1ScreenPos.Y <= InnerScreenSizeY || P2ScreenPos.Y <= InnerScreenDistance || P2ScreenPos.Y >= InnerScreenSizeY)
	{
		NewLength = CameraBoom->TargetArmLength - UpdateDistance;
	}

	if (NewLength != CameraBoom->TargetArmLength)
	{
		UE_LOG(LogBombermanIKA, Display, TEXT("CAMERA DISTANCE UPDATED: Previous [%.3f], New [%.3f]"), CameraBoom->TargetArmLength, NewLength);
		CameraBoom->TargetArmLength = FMath::Clamp(NewLength, 800.f, 2000.f);
	}
}