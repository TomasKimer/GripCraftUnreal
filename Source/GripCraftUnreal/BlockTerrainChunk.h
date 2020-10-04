// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Array3D.h"
#include "BlockTerrainChunk.generated.h"

UCLASS()
class GRIPCRAFTUNREAL_API ABlockTerrainChunk final : public AActor
{
	GENERATED_BODY()
	
public:	
	ABlockTerrainChunk();
	~ABlockTerrainChunk();

	void Initialize(int width, int height);
	void GenerateHeightmap(int PosX, int PosY, float NoiseScale, FVector2D NoiseOffset, class FastNoiseLite& NoiseLib);
	void UpdateMesh() const;

private:
	UPROPERTY(VisibleDefaultsOnly)
	class UProceduralMeshComponent* ProceduralMeshComponent;

	UPROPERTY(EditDefaultsOnly)
	class UMaterialInterface* Material;

	int Width, Height;
	TArray3D<bool>* BlockData;

	int GetTerrainHeight(int X, int Y, float NoiseScale, FVector2D NoiseOffset, class FastNoiseLite& NoiseLib) const;
	bool GetBlockTypeForHeight(int InHeight, int CurrMaxHeight) const;
	bool IsNone(int X, int Y, int Z) const;
	bool CheckBounds(int X, int Y, int Z) const;
	void CreateTestCube() const;

	static void AddFaceVertices(TArray<FVector>& Vertices, const FVector& Origin, const TArray<FVector>& VerticesToAdd);
};