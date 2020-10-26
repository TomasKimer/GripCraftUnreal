// Copyright Epic Games, Inc. All Rights Reserved.

#include "GripCraftUnrealGameMode.h"
#include "BlockTerrain/BlockTerrainManager.h"
#include "BlockTerrain/BlockTerrainSubsystem.h"
#include "EngineUtils.h"
#include "GripCraftUnrealGameInstance.h"
#include "GripCraftUnrealSaveGame.h"


AGripCraftUnrealGameMode::AGripCraftUnrealGameMode()
	: Super()
{
}

void AGripCraftUnrealGameMode::StartPlay()
{
	Super::StartPlay();

	ABlockTerrainManager* BlockTerrainManager = GetWorld()->GetSubsystem<UBlockTerrainSubsystem>()->GetManager();
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	APawn* PlayerPawn = PlayerController->GetPawn();
	FVector PlayerLocation;
	FRotator PlayerRotation;

	PlayerController->SetInputMode(FInputModeGameOnly());

	const bool bShouldLoadFromSave = GetGameInstance<UGripCraftUnrealGameInstance>()->ShouldLoadGameLevelFromSave();
	if (bShouldLoadFromSave && FGripCraftUnrealSaveGame::Load(*BlockTerrainManager, PlayerLocation, PlayerRotation))
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
