// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Array3D.h"
#include "BlockSettings.h"
#include "BlockTerrainChunk.generated.h"


struct FBlockData
{
	EBlockType BlockType;
	float Health;
};


UCLASS()
class GRIPCRAFTUNREAL_API ABlockTerrainChunk final : public AActor
{
	GENERATED_BODY()

public:
	ABlockTerrainChunk();
	~ABlockTerrainChunk();

	void Initialize(int width, int height, UBlockSettings* blockSettings);
	void GenerateHeightmap(int PosX, int PosY, float NoiseScale, FVector2D NoiseOffset, class FastNoiseLite& NoiseLib) const;
	void DamageBlock(int X, int Y, int Z, float damage);
	void UpdateMesh() const;

private:
	UPROPERTY(VisibleDefaultsOnly)
	class UProceduralMeshComponent* ProceduralMeshComponent;

	UPROPERTY(EditDefaultsOnly)
	class UMaterialInterface* Material;

	UPROPERTY()
	UBlockSettings* BlockSettings;

	int Width, Height;
	TArray3D<FBlockData>* BlockData;

	int GetTerrainHeight(int X, int Y, float NoiseScale, FVector2D NoiseOffset, class FastNoiseLite& NoiseLib) const;
	EBlockType GetBlockTypeForHeight(int InHeight, int CurrMaxHeight) const;
	void AddFaceVertices(TArray<FVector>& Vertices, const FVector& Origin, const TArray<FVector>& VerticesToAdd) const;
	bool IsNone(int X, int Y, int Z) const;
	bool CheckBounds(int X, int Y, int Z) const;
	void CreateTestCube() const;
};