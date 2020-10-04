// Fill out your copyright notice in the Description page of Project Settings.


#include "BlockTerrainManager.h"

#include "BlockSettings.h"
#include "BlockTerrainChunk.h"


ABlockTerrainManager::ABlockTerrainManager()
{
	PrimaryActorTick.bCanEverTick = true;
}


void ABlockTerrainManager::BeginPlay()
{
	Super::BeginPlay();

	NoiseLib.SetNoiseType(ConvertNoiseType(NoiseType));
}


void ABlockTerrainManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateChunks();
}


void ABlockTerrainManager::UpdateChunks()
{
	FVector   playerPosition      = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation(); // SP only, but it's enough for now
	FIntPoint playerChunkPosition = GetChunkPosition(playerPosition);

	if (playerChunkPosition == PlayerChunkPosition)
		return;

	PlayerChunkPosition = playerChunkPosition;

	UE_LOG(LogTemp, Display, TEXT("New player chunk position: X:%d Y:%d"), PlayerChunkPosition.X, PlayerChunkPosition.Y);

	CreateNewChunks();
}


void ABlockTerrainManager::CreateNewChunks()
{
	for (int x = PlayerChunkPosition.X - ChunkDistance; x <= PlayerChunkPosition.X + ChunkDistance; ++x)
	{
		for (int y = PlayerChunkPosition.Y - ChunkDistance; y <= PlayerChunkPosition.Y + ChunkDistance; ++y)
		{
			FIntPoint chunkPosition(x, y);

			if (ActiveChunks.Contains(chunkPosition) == false)
			{
				ActiveChunks.Add(chunkPosition, CreateChunk(chunkPosition));
			}
		}
	}
}


ABlockTerrainChunk* ABlockTerrainManager::CreateChunk(FIntPoint chunkPos)
{
	UE_LOG(LogTemp, Display, TEXT("Create chunk: X:%d Y:%d"), chunkPos.X, chunkPos.Y);

	int worldX = chunkPos.X * ChunkWidth;
	int worldY = chunkPos.Y * ChunkWidth;

	FVector worldPos(
		worldX * BlockSettings::BLOCK_SIZE,
		worldY * BlockSettings::BLOCK_SIZE,
		0
	);

	ABlockTerrainChunk* newChunk = GetWorld()->SpawnActor<ABlockTerrainChunk>(ChunkClassToSpawn, worldPos, FRotator::ZeroRotator);
	newChunk->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
	newChunk->SetActorLabel(FString(TEXT("Chunk ")) + chunkPos.ToString());

	newChunk->Initialize(ChunkWidth, ChunkHeight);
	newChunk->GenerateHeightmap(worldX, worldY, NoiseScale, NoiseOffset, NoiseLib);
	newChunk->UpdateMesh();

	return newChunk;
}


FIntPoint ABlockTerrainManager::GetChunkPosition(FVector position) const
{
	return FIntPoint(
		FMath::FloorToInt(position.X / (ChunkWidth * BlockSettings::BLOCK_SIZE)),
		FMath::FloorToInt(position.Y / (ChunkWidth * BlockSettings::BLOCK_SIZE))
	);
}


FastNoiseLite::NoiseType ABlockTerrainManager::ConvertNoiseType(ENoiseType NoiseType)
{
	switch (NoiseType)
	{
	case ENoiseType::OpenSimplex2:  return FastNoiseLite::NoiseType_OpenSimplex2;
	case ENoiseType::OpenSimplex2S: return FastNoiseLite::NoiseType_OpenSimplex2S;
	case ENoiseType::Cellular:      return FastNoiseLite::NoiseType_Cellular;
	case ENoiseType::Perlin:        return FastNoiseLite::NoiseType_Perlin;
	case ENoiseType::ValueCubic:    return FastNoiseLite::NoiseType_ValueCubic;
	case ENoiseType::Value:         return FastNoiseLite::NoiseType_Value;
	default: checkNoEntry();        return FastNoiseLite::NoiseType_Perlin;
	}
}