// Fill out your copyright notice in the Description page of Project Settings.


#include "BlockTerrainManager.h"
#include "BlockSettings.h"
#include "BlockTerrainChunk.h"
#include "BlockTerrainSubsystem.h"
#include "Kismet/GameplayStatics.h"


ABlockTerrainManager::ABlockTerrainManager()
{
	PrimaryActorTick.bCanEverTick = true;
}

ABlockTerrainManager::~ABlockTerrainManager()
{
	for (const auto& Pair : CachedBlockData)
		delete Pair.Value;
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
		(ChunkHeight + 1) * BlockSettings->BlockSize // + 9999
	);
}

void ABlockTerrainManager::AddBlock(const FVector AddBlockPosition, const EBlockType BlockType)
{
	const FIntPoint& ChunkPosition = GetChunkPosition(AddBlockPosition);

	ABlockTerrainChunk** Chunk = ActiveChunks.Find(ChunkPosition);
	if (Chunk == nullptr)
		return;

	const FIntVector& PositionInChunk = GetPositionInChunk(ChunkPosition, AddBlockPosition);

//	UE_LOG(LogTemp, Display, TEXT("Add block, Chunk pos: %s, BlockPos: %s, BlockType: %d"),
//            *ChunkPosition.ToString(), *PositionInChunk.ToString(), BlockType);

	(*Chunk)->SetBlock(PositionInChunk.X, PositionInChunk.Y, PositionInChunk.Z, BlockType);
}

void ABlockTerrainManager::DamageBlock(const FVector HitPosition, const FVector HitNormal, float Damage)
{
	const FVector& DamageBlockPosition = HitPosition - HitNormal * BlockSettings->BlockSize * 0.5f;
	const FIntPoint&     ChunkPosition = GetChunkPosition(DamageBlockPosition);

	ABlockTerrainChunk** Chunk = ActiveChunks.Find(ChunkPosition);
	if (Chunk == nullptr)
		return;

	const FIntVector& PositionInChunk = GetPositionInChunk(ChunkPosition, DamageBlockPosition);

//	UE_LOG(LogTemp, Display, TEXT("Damage block, Chunk pos: %s, BlockPos: %s, Damage: %f, Normal: %s"),
//            *ChunkPosition.ToString(), *PositionInChunk.ToString(), Damage, *HitNormal.ToString());

	(*Chunk)->DamageBlock(PositionInChunk.X, PositionInChunk.Y, PositionInChunk.Z, Damage);
}

void ABlockTerrainManager::UpdateChunks()
{
	FVector PlayerLocation;
	if (GetPlayerLocation(PlayerLocation) == false)
		return;

	const FIntPoint& CurrentPlayerChunkPosition = GetChunkPosition(PlayerLocation);
	if (CurrentPlayerChunkPosition == PlayerChunkPosition)
		return;

	PlayerChunkPosition = CurrentPlayerChunkPosition;

//	UE_LOG(LogTemp, Display, TEXT("New player chunk position: X:%d Y:%d"), PlayerChunkPosition.X, PlayerChunkPosition.Y);

	RemoveFarChunks();
	CreateNewChunks();
}

void ABlockTerrainManager::CreateNewChunks()
{
//	double startTime = FPlatformTime::Seconds();

	for (int x = PlayerChunkPosition.X - ChunkDistance; x <= PlayerChunkPosition.X + ChunkDistance; ++x)
	{
		for (int y = PlayerChunkPosition.Y - ChunkDistance; y <= PlayerChunkPosition.Y + ChunkDistance; ++y)
		{
			const FIntPoint ChunkPosition(x, y);

			if (ActiveChunks.Contains(ChunkPosition) == false)
			{
				ActiveChunks.Add(ChunkPosition, CreateChunk(ChunkPosition));
			}
		}
	}

//	UE_LOG(LogTemp, Warning, TEXT("Create new chunks took %f seconds"), FPlatformTime::Seconds() - startTime);
}

void ABlockTerrainManager::RemoveFarChunks()
{
	TArray<FIntPoint> ChunksToRemove;

	for (const auto& Pair : ActiveChunks)
	{
		const FIntPoint& ChunkPosition = Pair.Key;
		const FIntPoint& Diff = ChunkPosition - PlayerChunkPosition;

		if (FMath::Abs(Diff.X) <= ChunkDistance && FMath::Abs(Diff.Y) <= ChunkDistance)
			continue;

		ChunksToRemove.Emplace(ChunkPosition);
	}

	for (const FIntPoint& ChunkToRemove : ChunksToRemove)
	{
		ABlockTerrainChunk* Chunk = ActiveChunks[ChunkToRemove];
		Chunk->DeInitialize();

		if (Chunk->HasChanged() == true)
		{
			CachedBlockData.Add(ChunkToRemove, Chunk->TakeBlockData());
		}

		CachedChunks.Emplace(Chunk);
		ActiveChunks.Remove(ChunkToRemove);
	}
}

ABlockTerrainChunk* ABlockTerrainManager::CreateChunk(const FIntPoint ChunkPos)
{
	int WorldX = ChunkPos.X * ChunkWidth;
	int WorldY = ChunkPos.Y * ChunkWidth;

	const FVector WorldPos(
		WorldX * BlockSettings->BlockSize,
		WorldY * BlockSettings->BlockSize,
		0
	);

	ABlockTerrainChunk* NewChunk;
	if (CachedChunks.Num() > 0)
	{
//		UE_LOG(LogTemp, Display, TEXT("Recycle chunk: X:%d Y:%d"), ChunkPos.X, ChunkPos.Y);

		int LastIdx = CachedChunks.Num() - 1;
		NewChunk = CachedChunks[LastIdx];
		CachedChunks.RemoveAt(LastIdx);

		NewChunk->SetActorLocation(WorldPos);
	}
	else
	{
//		UE_LOG(LogTemp, Display, TEXT("Create chunk: X:%d Y:%d"), ChunkPos.X, ChunkPos.Y);

		NewChunk = GetWorld()->SpawnActor<ABlockTerrainChunk>(ChunkClassToSpawn, WorldPos, FRotator::ZeroRotator);
		NewChunk->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
	}

	TArray3D<FBlockData>** CachedData = CachedBlockData.Find(ChunkPos);

	NewChunk->Initialize(ChunkWidth, ChunkHeight, BlockSettings, CachedData != nullptr ? *CachedData : nullptr);
#if WITH_EDITOR
	NewChunk->SetActorLabel(FString(TEXT("Chunk ")) + ChunkPos.ToString());
#endif

	if (CachedData == nullptr)
	{
		NewChunk->GenerateHeightmap(WorldX, WorldY, NoiseScale, NoiseOffset, NoiseLib);
	}
	else
	{
		CachedBlockData.Remove(ChunkPos);
	}

	NewChunk->UpdateMesh();

	return NewChunk;
}

FIntVector ABlockTerrainManager::GetPositionInChunk(const FIntPoint& ChunkPosition, const FVector& TargetPosition) const
{
	return FIntVector(
		FMath::FloorToInt(TargetPosition.X / BlockSettings->BlockSize) - ChunkPosition.X * ChunkWidth,
		FMath::FloorToInt(TargetPosition.Y / BlockSettings->BlockSize) - ChunkPosition.Y * ChunkWidth,
		FMath::FloorToInt(TargetPosition.Z / BlockSettings->BlockSize)
    );
}

FIntPoint ABlockTerrainManager::GetChunkPosition(const FVector& Position) const
{
	return FIntPoint(
		FMath::FloorToInt(Position.X / (ChunkWidth * BlockSettings->BlockSize)),
		FMath::FloorToInt(Position.Y / (ChunkWidth * BlockSettings->BlockSize))
	);
}

bool ABlockTerrainManager::GetPlayerLocation(FVector& OutLocation) const
{
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (PlayerPawn == nullptr)
		return false;

	OutLocation = PlayerPawn->GetActorLocation();
	return true;
}

void ABlockTerrainManager::UpdateNoiseType()
{
	NoiseLib.SetNoiseType(ConvertNoiseType(NoiseType));
}

FastNoiseLite::NoiseType ABlockTerrainManager::ConvertNoiseType(const ENoiseType NoiseType)
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