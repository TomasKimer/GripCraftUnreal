// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Array3D.h"
#include "BlockData.h"
#include "BlockType.h"
#include "BlockTerrainChunk.generated.h"

class FBlockTerrainMeshDataTask;
class UBlockSettings;
class UMaterialInterface;
class UProceduralMeshComponent;
class FastNoiseLite;

UCLASS()
class GRIPCRAFTUNREAL_API ABlockTerrainChunk final : public AActor
{
	GENERATED_BODY()

public:
	ABlockTerrainChunk();
	~ABlockTerrainChunk();

	void Initialize(int InWidth, int InHeight, UBlockSettings* InBlockSettings, TArray3D<FBlockData>* InBlockData = nullptr);
	void DeInitialize();
	void GenerateHeightmap(int PosX, int PosY, float NoiseScale, const FVector2D NoiseOffset, FastNoiseLite& NoiseLib) const;
	void SetBlock(int X, int Y, int Z, const EBlockType BlockType);
	void DamageBlock(int X, int Y, int Z, float Damage);
	void UpdateMesh();
	FORCEINLINE bool HasChanged() const { return bHasChanged; }
	TArray3D<FBlockData>* TakeBlockData();
	FORCEINLINE TArray3D<FBlockData>* GetBlockData() const { return BlockData; }

private:
	UPROPERTY(VisibleDefaultsOnly)
	UProceduralMeshComponent* ProceduralMeshComponent;

	UPROPERTY(EditDefaultsOnly)
	UMaterialInterface* Material;

	UPROPERTY(EditDefaultsOnly)
	bool bUseAsyncGeneration = true;

	UPROPERTY()
	UBlockSettings* BlockSettings;

	int Width, Height;
	bool bHasChanged = false;
	TArray3D<FBlockData>* BlockData;
	FAsyncTask<FBlockTerrainMeshDataTask>* MeshDataTask;

	virtual void Tick(float DeltaSeconds) override;

	void TryFinishMeshDataTask();
	void TryCancelMeshDataTask();
	int GetTerrainHeight(int X, int Y, float NoiseScale, const FVector2D NoiseOffset, FastNoiseLite& NoiseLib) const;
	EBlockType GetBlockTypeForHeight(int InHeight, int CurrMaxHeight) const;
	bool CheckBounds(int X, int Y, int Z) const;
};
