// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FastNoiseLite.h"
#include "Array3D.h"
#include "BlockTerrainChunk.generated.h"

UCLASS()
class GRIPCRAFTUNREAL_API ABlockTerrainChunk final : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABlockTerrainChunk();
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleDefaultsOnly)
	class UProceduralMeshComponent* ProceduralMeshComp;

	UPROPERTY(EditAnywhere)
	class UMaterialInterface* Material;
	
	FastNoiseLite NoiseLib;
	int Width = 16;
	int Height = 32;
	float PerlinScale = 1.f; //0.02945f;
	FVector2D PerlinOffset = FVector2D(73.73f, 6562.0f);
	TArray3D<bool>* BlockData;

	void GenerateHeightmap(int PosX, int PosY);
	void UpdateMesh() const;
	int GetTerrainHeight(int X, int Y);	
	bool GetBlockTypeForHeight(int InHeight, int CurrMaxHeight) const;
	bool IsNone(int X, int Y, int Z) const;
	bool CheckBounds(int X, int Y, int Z) const;	
	void CreateTestCube() const;

	static void AddFaceVertices(TArray<FVector>& Vertices, const FVector& Origin, const TArray<FVector>& VerticesToAdd);
};