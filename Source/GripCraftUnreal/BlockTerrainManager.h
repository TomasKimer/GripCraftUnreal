// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BlockSettings.h"
#include "BlockTerrainChunk.h"
#include "GameFramework/Actor.h"
#include "FastNoiseLite.h"
#include "BlockTerrainManager.generated.h"


UENUM()
enum class ENoiseType : uint8
{
	OpenSimplex2,
	OpenSimplex2S,
	Cellular,
	Perlin,
	ValueCubic,
	Value
};


UCLASS()
class GRIPCRAFTUNREAL_API ABlockTerrainManager final : public AActor
{
	GENERATED_BODY()

public:
	ABlockTerrainManager();
	~ABlockTerrainManager();

	virtual void Tick(float DeltaTime) override;

	FVector GetOptimalPlayerSpawnLocation() const;
	void AddBlock(FVector AddBlockPosition, EBlockType BlockType);
	void DamageBlock(FVector HitPosition, FVector HitNormal, float Damage);

	friend FArchive& operator<<(FArchive& Ar, ABlockTerrainManager& BlockTerrainManager);

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, meta = (ClampMin = "8", ClampMax = "128"))
	int ChunkWidth = 16;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "8", ClampMax = "128"))
	int ChunkHeight = 32;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "1", ClampMax = "32"))
	int ChunkDistance = 5;

	UPROPERTY(EditAnywhere)
	ENoiseType NoiseType = ENoiseType::Perlin;

	UPROPERTY(EditAnywhere)
	float NoiseScale = 2.945f; //0.02945f;

	UPROPERTY(EditAnywhere)
	FVector2D NoiseOffset = FVector2D(73.73f, 6562.0f);

	UPROPERTY(EditAnywhere)
	TSubclassOf<ABlockTerrainChunk> ChunkClassToSpawn;

	UPROPERTY(EditDefaultsOnly)
	UBlockSettings* BlockSettings;

	UPROPERTY()
	TMap<FIntPoint, ABlockTerrainChunk*> ActiveChunks;

	UPROPERTY()
	TArray<ABlockTerrainChunk*> CachedChunks;

	TMap<FIntPoint, TArray3D<FBlockData>*> CachedBlockData;
	FastNoiseLite NoiseLib;
	FIntPoint PlayerChunkPosition = FIntPoint(TNumericLimits<int32>::Min(), TNumericLimits<int32>::Min());

	void UpdateChunks();
	void CreateNewChunks();
	void RemoveFarChunks();
	ABlockTerrainChunk* CreateChunk(FIntPoint ChunkPos);
	FIntVector GetPositionInChunk(FIntPoint ChunkPosition, FVector TargetPosition) const;
	FIntPoint GetChunkPosition(FVector Position) const;
	bool GetPlayerLocation(FVector& OutLocation) const;
	void UpdateNoiseType();

	static FastNoiseLite::NoiseType ConvertNoiseType(ENoiseType NoiseType);
};
