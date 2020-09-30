// Copyright Epic Games, Inc. All Rights Reserved.

#include "GripCraftUnrealGameMode.h"
#include "GripCraftUnrealHUD.h"
#include "GripCraftUnrealCharacter.h"
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
