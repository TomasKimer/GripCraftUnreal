// Copyright Epic Games, Inc. All Rights Reserved.

#include "GripCraftUnrealGameMode.h"
#include "BlockTerrainManager.h"
#include "BlockTerrainSubsystem.h"
#include "EngineUtils.h"
#include "GripCraftUnrealHUD.h"
#include "GripCraftUnrealSaveGame.h"
#include "UObject/ConstructorHelpers.h"


AGripCraftUnrealGameMode::AGripCraftUnrealGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AGripCraftUnrealHUD::StaticClass();
}

void AGripCraftUnrealGameMode::StartPlay()
{
	Super::StartPlay();

	ABlockTerrainManager* BlockTerrainManager = GetWorld()->GetSubsystem<UBlockTerrainSubsystem>()->GetManager();
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	APawn* PlayerPawn = PlayerController->GetPawn();
	FVector PlayerLocation;
	FRotator PlayerRotation;

	if (FGripCraftUnrealSaveGame::Load(*BlockTerrainManager, PlayerLocation, PlayerRotation) == true)
	{
		PlayerPawn->SetActorLocation(PlayerLocation);
		PlayerController->SetControlRotation(PlayerRotation);

		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Green, TEXT("Game Loaded."));
	}
	else
	{
		const FVector& OptimalSpawnActorLocation = BlockTerrainManager->GetOptimalPlayerSpawnLocation();

		PlayerPawn->SetActorLocation(OptimalSpawnActorLocation);
	}
}
