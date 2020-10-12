// Fill out your copyright notice in the Description page of Project Settings.


#include "BlockTerrainManager.h"
#include "BlockSettings.h"
#include "BlockTerrainChunk.h"
#include "BlockTerrainSubsystem.h"
#include "GripCraftUnrealSaveGame.h"


ABlockTerrainManager::ABlockTerrainManager()
{
	PrimaryActorTick.bCanEverTick = true;
}


ABlockTerrainManager::~ABlockTerrainManager()
{
	for (const auto& pair : CachedBlockData)
		delete pair.Value;
}


void ABlockTerrainManager::BeginPlay()
{
	Super::BeginPlay();

	GetWorld()->GetSubsystem<UBlockTerrainSubsystem>()->RegisterManager(this);

	UpdateNoiseType();
}


void ABlockTerrainManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateChunks();
}


FVector ABlockTerrainManager::GetOptimalPlayerSpawnLocation() const
{
	return FVector(
		ChunkWidth * BlockSettings->BlockSize / 2.0f,
		ChunkWidth * BlockSettings->BlockSize / 2.0f,
		(ChunkHeight + 1) * BlockSettings->BlockSize
	);
}


void ABlockTerrainManager::AddBlock(FVector AddBlockPosition, EBlockType BlockType)
{
	FIntPoint chunkPosition = GetChunkPosition(AddBlockPosition);

	ABlockTerrainChunk** chunk = ActiveChunks.Find(chunkPosition);
	if (chunk == nullptr)
		return;

	FIntVector positionInChunk = GetPositionInChunk(chunkPosition, AddBlockPosition);

	UE_LOG(LogTemp, Display, TEXT("Add block, Chunk pos: %s, BlockPos: %s, BlockType: %d"),
            *chunkPosition.ToString(), *positionInChunk.ToString(), BlockType);

	(*chunk)->SetBlock(positionInChunk.X, positionInChunk.Y, positionInChunk.Z, BlockType);
}


void ABlockTerrainManager::DamageBlock(FVector HitPosition, FVector HitNormal, float Damage)
{
	FVector damageBlockPosition = HitPosition - HitNormal * BlockSettings->BlockSize * 0.5f;
	FIntPoint     chunkPosition = GetChunkPosition(damageBlockPosition);

	ABlockTerrainChunk** chunk = ActiveChunks.Find(chunkPosition);
	if (chunk == nullptr)
		return;

	FIntVector positionInChunk = GetPositionInChunk(chunkPosition, damageBlockPosition);

	UE_LOG(LogTemp, Display, TEXT("Damage block, Chunk pos: %s, BlockPos: %s, Damage: %f, Normal: %s"),
            *chunkPosition.ToString(), *positionInChunk.ToString(), Damage, *HitNormal.ToString());

	(*chunk)->DamageBlock(positionInChunk.X, positionInChunk.Y, positionInChunk.Z, Damage);
}


void ABlockTerrainManager::UpdateChunks()
{
	const FIntPoint playerChunkPosition = GetChunkPosition(GetPlayerLocation());
	if (playerChunkPosition == PlayerChunkPosition)
		return;

	PlayerChunkPosition = playerChunkPosition;

//	UE_LOG(LogTemp, Display, TEXT("New player chunk position: X:%d Y:%d"), PlayerChunkPosition.X, PlayerChunkPosition.Y);

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
		if (chunk->HasChanged() == true)
		{
			CachedBlockData.Add(chunkToRemove, chunk->TakeBlockData());
		}

		CachedChunks.Emplace(chunk);
		ActiveChunks.Remove(chunkToRemove);
	}
}


ABlockTerrainChunk* ABlockTerrainManager::CreateChunk(FIntPoint ChunkPos)
{
	int worldX = ChunkPos.X * ChunkWidth;
	int worldY = ChunkPos.Y * ChunkWidth;

	FVector worldPos(
		worldX * BlockSettings->BlockSize,
		worldY * BlockSettings->BlockSize,
		0
	);

	ABlockTerrainChunk* newChunk;
	if (CachedChunks.Num() > 0)
	{
//		UE_LOG(LogTemp, Display, TEXT("Recycle chunk: X:%d Y:%d"), chunkPos.X, chunkPos.Y);

		int lastIdx = CachedChunks.Num() - 1;
		newChunk = CachedChunks[lastIdx];
		CachedChunks.RemoveAt(lastIdx);

		newChunk->SetActorLocation(worldPos);
	}
	else
	{
//		UE_LOG(LogTemp, Display, TEXT("Create chunk: X:%d Y:%d"), chunkPos.X, chunkPos.Y);

		newChunk = GetWorld()->SpawnActor<ABlockTerrainChunk>(ChunkClassToSpawn, worldPos, FRotator::ZeroRotator);
		newChunk->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
	}

	TArray3D<FBlockData>** cachedBlockData = CachedBlockData.Find(ChunkPos);

	newChunk->Initialize(ChunkWidth, ChunkHeight, BlockSettings, cachedBlockData != nullptr ? *cachedBlockData : nullptr);
	newChunk->SetActorLabel(FString(TEXT("Chunk ")) + ChunkPos.ToString());

	if (cachedBlockData == nullptr)
	{
		newChunk->GenerateHeightmap(worldX, worldY, NoiseScale, NoiseOffset, NoiseLib);	
	}
	else
	{
		CachedBlockData.Remove(ChunkPos);
	}

	newChunk->UpdateMesh();

	return newChunk;
}


FIntVector ABlockTerrainManager::GetPositionInChunk(FIntPoint ChunkPosition, FVector TargetPosition) const
{
	return FIntVector(
    FMath::FloorToInt(TargetPosition.X / BlockSettings->BlockSize) - ChunkPosition.X * ChunkWidth,
    FMath::FloorToInt(TargetPosition.Y / BlockSettings->BlockSize) - ChunkPosition.Y * ChunkWidth,
    FMath::FloorToInt(TargetPosition.Z / BlockSettings->BlockSize)
    );
}


FIntPoint ABlockTerrainManager::GetChunkPosition(FVector Position) const
{
	return FIntPoint(
		FMath::FloorToInt(Position.X / (ChunkWidth * BlockSettings->BlockSize)),
		FMath::FloorToInt(Position.Y / (ChunkWidth * BlockSettings->BlockSize))
	);
}


FVector ABlockTerrainManager::GetPlayerLocation() const  // SP only, but it's enough for now
{
	const APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController == nullptr)
		return FVector();

	APawn* PlayerPawn = PlayerController->GetPawn();
	if (PlayerPawn == nullptr)
		return FVector();

	return PlayerPawn->GetActorLocation();
}


void ABlockTerrainManager::UpdateNoiseType()
{
	NoiseLib.SetNoiseType(ConvertNoiseType(NoiseType));
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


FArchive& operator<<(FArchive& Ar, ABlockTerrainManager& BlockTerrainManager)
{
	Ar << BlockTerrainManager.ChunkWidth;
	Ar << BlockTerrainManager.ChunkHeight;
	Ar << BlockTerrainManager.NoiseType;
	Ar << BlockTerrainManager.NoiseScale;
	Ar << BlockTerrainManager.NoiseOffset;

	if (Ar.IsLoading() == true)
	{
		Ar << BlockTerrainManager.CachedBlockData;

		BlockTerrainManager.UpdateNoiseType();
	}
	else
	{
		TMap<FIntPoint, TArray3D<FBlockData>*> SaveBlockData;

		for (const TTuple<FIntPoint, ABlockTerrainChunk*>& Pair : BlockTerrainManager.ActiveChunks)
		{
			if (Pair.Value->HasChanged() == false)
				continue;

			SaveBlockData.Add(Pair.Key, Pair.Value->GetBlockData());
		}

		SaveBlockData.Append(BlockTerrainManager.CachedBlockData);

		Ar << SaveBlockData;
	}

	return Ar;
}