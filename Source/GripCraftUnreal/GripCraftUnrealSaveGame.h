// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BlockTerrainManager.h"
#include "GameFramework/SaveGame.h"
#include "Serialization/BufferArchive.h"
#include "GripCraftUnrealSaveGame.generated.h"


UCLASS()
class GRIPCRAFTUNREAL_API UGripCraftUnrealSaveGame final : public USaveGame
{
	GENERATED_BODY()

	static void SaveLoad(FArchive& Ar, ABlockTerrainManager& BlockTerrainManager, FVector& PlayerPos, FRotator& PlayerRot);

public:
	static const FString DefaultSlotName;
	static const int32   DefaultUserIndex;

	static void Save(ABlockTerrainManager& BlockTerrainManager, FVector PlayerPos, FRotator PlayerRot);
	static bool Load(ABlockTerrainManager& BlockTerrainManager, FVector& PlayerPos, FRotator& PlayerRot);
};
