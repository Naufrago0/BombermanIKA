#include "BIKBombActor.h"
#include "BombermanIKAPlayerController.h"
#include "BombermanIKACharacter.h"
#include "BIKLevelBlock.h"
#include "Components/StaticMeshComponent.h"

ABIKBombActor::ABIKBombActor()
{
	LevelBlock = nullptr;
	RemainingSeconds = 0.f;

	//Enable tick for this class of actors
	PrimaryActorTick.bCanEverTick = true;
}

void ABIKBombActor::ConfigureBomb(struct FBIKLevelBlock* LevelBlockArg)
{
	check(LevelBlockArg != nullptr);
	auto BIKCharacter = Cast<ABombermanIKACharacter>(Instigator);
	check(BIKCharacter != nullptr);
	auto BIKPC = Cast<ABombermanIKAPlayerController>(BIKCharacter->GetController());
	check(BIKPC != nullptr);

	UStaticMeshComponent* SMComp = Cast<UStaticMeshComponent>(GetComponentByClass(UStaticMeshComponent::StaticClass()));
	if(SMComp != nullptr)
	{
		// if this bomb belongs to player 1 ignore him and block player 2 and viceversa
		bool bIsFirstPlayer = BIKPC->IsFirstPlayer();
		SMComp->SetCollisionObjectType(bIsFirstPlayer ? ECollisionChannel::ECC_GameTraceChannel3 : ECollisionChannel::ECC_GameTraceChannel4);
		SMComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, bIsFirstPlayer ? ECollisionResponse::ECR_Ignore : ECollisionResponse::ECR_Block);
		SMComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, bIsFirstPlayer ? ECollisionResponse::ECR_Block : ECollisionResponse::ECR_Ignore);
	}

	// Store the level block the bomb is on
	LevelBlock = LevelBlockArg;

	LevelBlock->BombSpawned(this);

	// Set time to explosion
	RemainingSeconds = TimeToExplosionSeconds;
}

void ABIKBombActor::Tick(float DeltaSeconds)
{
	if (RemainingSeconds > 0.f)
	{
		RemainingSeconds -= DeltaSeconds;
		if (RemainingSeconds <= 0.f)
		{
			ExplodeBomb();
		}
	}
}

void ABIKBombActor::ExplodeBomb()
{
	// TODO Calculate all level blocks affected
	LevelBlock->ExplosionSpawned(nullptr); //TODO pass the explosion created
	auto BIKPC = Cast<ABombermanIKAPlayerController>(Instigator->GetController());
	check(BIKPC != nullptr);
	BIKPC->OnBombExploded();
	Destroy();
}