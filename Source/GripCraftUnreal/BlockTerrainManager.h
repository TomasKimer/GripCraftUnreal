// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "BlockTerrainChunk.h"
#include "GameFramework/Actor.h"
#include "BlockTerrainManager.generated.h"

UCLASS()
class GRIPCRAFTUNREAL_API ABlockTerrainManager final : public AActor
{
	GENERATED_BODY()

public:
	ABlockTerrainManager();

	virtual void Tick(float DeltaTime) override;

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
	float PerlinScale = 1.f; //0.02945f;

	UPROPERTY(EditAnywhere)
	FVector2D PerlinOffset = FVector2D(73.73f, 6562.0f);

private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<ABlockTerrainChunk> ChunkClassToSpawn;

	UPROPERTY()
	TMap<FIntPoint, class ABlockTerrainChunk*> ActiveChunks;

	FIntPoint PlayerChunkPosition = FIntPoint(INT_MIN, INT_MIN);

	void UpdateChunks();
	void CreateNewChunks();
	class ABlockTerrainChunk* CreateChunk(FIntPoint chunkPos);
	FIntPoint GetChunkPosition(FVector position) const;
};
