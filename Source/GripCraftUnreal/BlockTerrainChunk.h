// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Array3D.h"
#include "BlockSettings.h"
#include "BlockTerrainMeshDataTask.h"
#include "BlockTerrainChunk.generated.h"


UCLASS()
class GRIPCRAFTUNREAL_API ABlockTerrainChunk final : public AActor
{
	GENERATED_BODY()

public:
	ABlockTerrainChunk();
	~ABlockTerrainChunk();

	void Initialize(int InWidth, int InHeight, /*const*/ UBlockSettings* InBlockSettings, TArray3D<FBlockData>* InBlockData = nullptr);
	void DeInitialize();
	void GenerateHeightmap(int PosX, int PosY, float NoiseScale, FVector2D NoiseOffset, class FastNoiseLite& NoiseLib) const;
	void SetBlock(int X, int Y, int Z, EBlockType BlockType);
	void DamageBlock(int X, int Y, int Z, float Damage);
	void UpdateMesh(); //const;
	FORCEINLINE bool HasChanged() const { return bChanged; }
	TArray3D<FBlockData>* TakeBlockData();
	FORCEINLINE TArray3D<FBlockData>* GetBlockData() const { return BlockData; }

	virtual void Tick(float DeltaSeconds) override;

private:
	UPROPERTY(VisibleDefaultsOnly)
	class UProceduralMeshComponent* ProceduralMeshComponent;

	UPROPERTY(EditDefaultsOnly)
	class UMaterialInterface* Material;

	UPROPERTY(EditDefaultsOnly)
	bool bUseAsyncGeneration = true;

	UPROPERTY()
	UBlockSettings* BlockSettings; //const

	int Width, Height;
	bool bChanged = false;
	TArray3D<FBlockData>* BlockData;
	FAsyncTask<FBlockTerrainMeshDataTask>* MeshDataTask;

	void TryFinishMeshDataTask();
	void TryCancelMeshDataTask();
	int GetTerrainHeight(int X, int Y, float NoiseScale, FVector2D NoiseOffset, class FastNoiseLite& NoiseLib) const;
	EBlockType GetBlockTypeForHeight(int InHeight, int CurrMaxHeight) const;
	bool CheckBounds(int X, int Y, int Z) const;
};
