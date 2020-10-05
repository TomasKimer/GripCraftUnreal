// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GripCraftUnrealGameMode.generated.h"

UCLASS(minimalapi)
class AGripCraftUnrealGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AGripCraftUnrealGameMode();

	virtual void StartPlay() override;
};



