// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BlockTerrainManager.h"
#include "Serialization/BufferArchive.h"

class FGripCraftUnrealSaveGame
{
	static void SaveLoad(FArchive& Ar, ABlockTerrainManager& BlockTerrainManager, FVector& PlayerPos, FRotator& PlayerRot);

public:
	static const FString DefaultSlotName;
	static const int32   DefaultUserIndex;

	static void Save(ABlockTerrainManager& BlockTerrainManager, FVector PlayerPos, FRotator PlayerRot);
	static bool Load(ABlockTerrainManager& BlockTerrainManager, FVector& PlayerPos, FRotator& PlayerRot);
};
