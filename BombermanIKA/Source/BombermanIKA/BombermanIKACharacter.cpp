// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "BombermanIKACharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BombermanIKAPlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Materials/Material.h"
#include "Components/SkeletalMeshComponent.h"

ABombermanIKACharacter::ABombermanIKACharacter()
{
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(40.f, 96.f);

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	//// Create a camera boom...
	//CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	//CameraBoom->SetupAttachment(RootComponent);
	//CameraBoom->bAbsoluteRotation = true; // Don't want arm to rotate when character does
	//CameraBoom->TargetArmLength = 800.f;
	//CameraBoom->RelativeRotation = FRotator(-60.f, 0.f, 0.f);
	//CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	//// Create a camera...
	//TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	//TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	//TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void ABombermanIKACharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

}

void ABombermanIKACharacter::ConfigureP2Character()
{
	auto SKMeshComp = GetMesh();
	auto CapsuleComp = GetCapsuleComponent();

	if (SKMeshComp != nullptr)
	{
		SKMeshComp->SetMaterial(0, P2Material);
		// Set Collision Object Type to PlayerCharacter2 according to DefaultEngine.ini
		SKMeshComp->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel2);

		// Collide against player 1
		SKMeshComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Block);
		// Ignore other player 2, not likey to happen.
		SKMeshComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Ignore);
		// Collide agains player 1's bombs
		SKMeshComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel3, ECollisionResponse::ECR_Block);
		// Ignore own bombs
		SKMeshComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel4, ECollisionResponse::ECR_Ignore);
	}

	// Same collision configuration for capsule component
	if (CapsuleComp != nullptr)
	{
		// Set Collision Object Type to PlayerCharacter2 according to DefaultEngine.ini
		CapsuleComp->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel2);

		// Collide against player 1
		CapsuleComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Block);
		// Ignore other player 2, not likey to happen.
		CapsuleComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Ignore);
		// Collide agains player 1's bombs
		CapsuleComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel3, ECollisionResponse::ECR_Block);
		// Ignore own bombs
		CapsuleComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel4, ECollisionResponse::ECR_Ignore);
	}
}
