// Fill out your copyright notice in the Description page of Project Settings.


#include "GripCraftUnrealSaveGame.h"
#include "Kismet/GameplayStatics.h"


const FString UGripCraftUnrealSaveGame::DefaultSlotName  = "slot04";
const int     UGripCraftUnrealSaveGame::DefaultUserIndex = 0;

void UGripCraftUnrealSaveGame::Save(ABlockTerrainManager& BlockTerrainManager, FVector PlayerPos, FRotator PlayerRot)
{
	FBufferArchive Archive;
	SaveLoad(Archive, BlockTerrainManager, PlayerPos, PlayerRot);

	UGameplayStatics::SaveDataToSlot(Archive, DefaultSlotName, DefaultUserIndex);
}

bool UGripCraftUnrealSaveGame::Load(ABlockTerrainManager& BlockTerrainManager, FVector& PlayerPos, FRotator& PlayerRot)
{
	if (UGameplayStatics::DoesSaveGameExist(DefaultSlotName, DefaultUserIndex) == false)
		return false;

	TArray<uint8> DataArray;
	UGameplayStatics::LoadDataFromSlot(DataArray, DefaultSlotName, DefaultUserIndex);

	FMemoryReader Reader(DataArray);
	SaveLoad(Reader, BlockTerrainManager, PlayerPos, PlayerRot);

	return true;
}

void UGripCraftUnrealSaveGame::SaveLoad(FArchive& Ar, ABlockTerrainManager& BlockTerrainManager, FVector& PlayerPos, FRotator& PlayerRot)
{
	Ar << BlockTerrainManager;
	Ar << PlayerPos;
	Ar << PlayerRot;
}
