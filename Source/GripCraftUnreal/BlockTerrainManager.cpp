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

	RemoveFarChunks();
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


void ABlockTerrainManager::RemoveFarChunks()
{
	TArray<FIntPoint> chunksToRemove;

	for (const auto& pair : ActiveChunks)
	{
		FIntPoint chunkPosition = pair.Key;
		FIntPoint diff = chunkPosition - PlayerChunkPosition;
		if (FMath::Abs(diff.X) <= ChunkDistance && FMath::Abs(diff.Y) <= ChunkDistance)
			continue;

		chunksToRemove.Emplace(chunkPosition);
	}

	for (const FIntPoint& chunkToRemove : chunksToRemove)
	{
		ABlockTerrainChunk* chunk = ActiveChunks[chunkToRemove];

		CachedChunks.Emplace(chunk);
		ActiveChunks.Remove(chunkToRemove);
	}
}


ABlockTerrainChunk* ABlockTerrainManager::CreateChunk(FIntPoint chunkPos)
{
	int worldX = chunkPos.X * ChunkWidth;
	int worldY = chunkPos.Y * ChunkWidth;

	FVector worldPos(
		worldX * BlockSettings::BLOCK_SIZE,
		worldY * BlockSettings::BLOCK_SIZE,
		0
	);

	ABlockTerrainChunk* newChunk;
	if (CachedChunks.Num() > 0)
	{
		UE_LOG(LogTemp, Display, TEXT("Recycle chunk: X:%d Y:%d"), chunkPos.X, chunkPos.Y);

		int lastIdx = CachedChunks.Num() - 1;
		newChunk = CachedChunks[lastIdx];
		CachedChunks.RemoveAt(lastIdx);

		newChunk->SetActorLocation(worldPos);
	}
	else
	{
		UE_LOG(LogTemp, Display, TEXT("Create chunk: X:%d Y:%d"), chunkPos.X, chunkPos.Y);

		newChunk = GetWorld()->SpawnActor<ABlockTerrainChunk>(ChunkClassToSpawn, worldPos, FRotator::ZeroRotator);
		newChunk->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
		newChunk->Initialize(ChunkWidth, ChunkHeight);
	}

	newChunk->SetActorLabel(FString(TEXT("Chunk ")) + chunkPos.ToString());	
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