// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BlockTerrainManager.h"
#include "Subsystems/WorldSubsystem.h"
#include "BlockTerrainSubsystem.generated.h"

UCLASS()
class GRIPCRAFTUNREAL_API UBlockTerrainSubsystem final : public UWorldSubsystem
{
	GENERATED_BODY()

	UPROPERTY()
	ABlockTerrainManager* BlockTerrainManager;

public:
	void RegisterManager(ABlockTerrainManager* Manager)
	{
		BlockTerrainManager = Manager;
	}

	FORCEINLINE ABlockTerrainManager* GetManager() const
	{
		return BlockTerrainManager;
	}
};
