// Fill out your copyright notice in the Description page of Project Settings.


#include "BlockTerrainChunk.h"
#include "ProceduralMeshComponent.h"
#include "KismetProceduralMeshLibrary.h"
#include "FastNoiseLite.h"
#include "BlockSettings.h"


ABlockTerrainChunk::ABlockTerrainChunk()
{
	PrimaryActorTick.bCanEverTick = false;

	ProceduralMeshComponent = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProceduralMeshComponent"));
	RootComponent = ProceduralMeshComponent;
}


ABlockTerrainChunk::~ABlockTerrainChunk()
{
	delete BlockData;
}


void ABlockTerrainChunk::Initialize(int width, int height, UBlockSettings* blockSettings)
{
	Width = width;
	Height = height;
	BlockSettings = blockSettings;

	BlockData = new TArray3D<bool>(Width, Width, Height);
}


void ABlockTerrainChunk::GenerateHeightmap(int PosX, int PosY, float NoiseScale, FVector2D NoiseOffset, FastNoiseLite& NoiseLib) const
{
	for (int x = 0; x < Width; ++x)
	{
		for (int y = 0; y < Width; ++y)
		{
			int height = GetTerrainHeight(PosX + x, PosY + y, NoiseScale, NoiseOffset, NoiseLib);

			for (int z = 0; z < Height; ++z)
			{
				BlockData->Get(x, y, z) = GetBlockTypeForHeight(z, height);
			}
		}
	}
}


void ABlockTerrainChunk::UpdateMesh() const
{
	const int VERTICES_PER_FACE = 4;

	TArray<FVector> vertices;
	TArray<int32> triangles;
	TArray<FVector2D> uvs;

	for (int x = 0; x < Width; ++x)
	{
		for (int y = 0; y < Width; ++y)
		{
			for (int z = 0; z < Height; ++z)
			{
				bool isBlock = BlockData->Get(x, y, z);
				if (isBlock == false)
					continue;

				int faceCount = 0;
				FVector pos(x, y, z);

				if (IsNone(x - 1, y, z))
				{
					AddFaceVertices(vertices, pos, UBlockSettings::LEFT_VERTICES);
					uvs.Append(UBlockSettings::TEST_UVS);
					faceCount += 1;
				}

				if (IsNone(x + 1, y, z))
				{
					AddFaceVertices(vertices, pos, UBlockSettings::RIGHT_VERTICES);
					uvs.Append(UBlockSettings::TEST_UVS);
					faceCount += 1;
				}

				if (IsNone(x, y, z - 1))
				{
					AddFaceVertices(vertices, pos, UBlockSettings::BOTTOM_VERTICES);
					uvs.Append(UBlockSettings::TEST_UVS);
					faceCount += 1;
				}

				if (IsNone(x, y, z + 1))
				{
					AddFaceVertices(vertices, pos, UBlockSettings::TOP_VERTICES);
					uvs.Append(UBlockSettings::TEST_UVS);
					faceCount += 1;
				}

				if (IsNone(x, y + 1, z))
				{
					AddFaceVertices(vertices, pos, UBlockSettings::BACK_VERTICES);
					uvs.Append(UBlockSettings::TEST_UVS);
					faceCount += 1;
				}

				if (IsNone(x, y - 1, z))
				{
					AddFaceVertices(vertices, pos, UBlockSettings::FRONT_VERTICES);
					uvs.Append(UBlockSettings::TEST_UVS);
					faceCount += 1;
				}

				int startIdx = vertices.Num() - faceCount * VERTICES_PER_FACE;
				for (int i = 0; i < faceCount; ++i)
				{
					int idx = startIdx + i * VERTICES_PER_FACE;

					triangles.Add(idx);
					triangles.Add(idx + 1);
					triangles.Add(idx + 2);

					triangles.Add(idx);
					triangles.Add(idx + 2);
					triangles.Add(idx + 3);
				}
			}
		}
	}

	TArray<FVector> normals;
	TArray<FProcMeshTangent> tangents;
	TArray<FLinearColor> vertexColors;

	// TODO looks weird, something must be wrong here (and is way toooo slow)
//	UKismetProceduralMeshLibrary::CalculateTangentsForMesh(vertices, triangles, uvs, normals, tangents);

	ProceduralMeshComponent->ClearMeshSection(0);
	ProceduralMeshComponent->CreateMeshSection_LinearColor(0, vertices, triangles, normals, uvs, vertexColors, tangents, true);
	ProceduralMeshComponent->SetMaterial(0, Material);
}


int ABlockTerrainChunk::GetTerrainHeight(int X, int Y, float NoiseScale, FVector2D NoiseOffset, FastNoiseLite& NoiseLib) const
{
	float noiseCoordX = X * NoiseScale + NoiseOffset.X;
	float noiseCoordY = Y * NoiseScale + NoiseOffset.Y;
	float noiseSample = (NoiseLib.GetNoise(noiseCoordX, noiseCoordY) + 1.f) * 0.5f;

	int result = FMath::Clamp(FMath::FloorToInt(noiseSample * Height), 0, Height - 1);

//	UE_LOG(LogTemp, Display, TEXT("X:%f Y:%f Sample:%f Result:%d"), noiseCoordX, noiseCoordY, noiseSample, result);

	return result;
}


bool ABlockTerrainChunk::GetBlockTypeForHeight(int InHeight, int CurrMaxHeight) const
{
	if (InHeight > CurrMaxHeight)
		return false;

	float fraction = InHeight / static_cast<float>(Height);

	if (fraction < 0.2f)
		return true; // stone
	if (fraction < 0.4f)
		return true; // dirt
	if (fraction < 0.6f)
		return true; // grass

	return true; // snow
}


void ABlockTerrainChunk::AddFaceVertices(TArray<FVector>& Vertices, const FVector& Origin, const TArray<FVector>& VerticesToAdd) const
{
	for (int idx = 0; idx < VerticesToAdd.Num(); ++idx)
	{
		Vertices.Add((Origin + VerticesToAdd[idx]) * BlockSettings->BlockSize);
	}
}


bool ABlockTerrainChunk::IsNone(int X, int Y, int Z) const
{
	if (CheckBounds(X, Y, Z) == false)
		return true;
	
	return BlockData->Get(X, Y, Z) == false;
}


bool ABlockTerrainChunk::CheckBounds(int X, int Y, int Z) const
{
	if (X < 0 || Y < 0 || Z < 0)
		return false;
	if (X >= Width || Y >= Width || Z >= Height)
		return false;

	return true;
}


void ABlockTerrainChunk::CreateTestCube() const
{
	const int   FACE_COUNT        = 6;
	const int   INDICES_PER_FACE  = 6;
	const int   VERTICES_PER_FACE = 4;
	const int   VERTEX_COUNT      = FACE_COUNT * VERTICES_PER_FACE;
	const int   INDEX_COUNT       = FACE_COUNT * INDICES_PER_FACE;

	TArray<FVector> vertices;
	vertices.Reserve(VERTEX_COUNT);

	vertices.Append(UBlockSettings::LEFT_VERTICES);
	vertices.Append(UBlockSettings::RIGHT_VERTICES);
	vertices.Append(UBlockSettings::FRONT_VERTICES);
	vertices.Append(UBlockSettings::BACK_VERTICES);
	vertices.Append(UBlockSettings::TOP_VERTICES);
	vertices.Append(UBlockSettings::BOTTOM_VERTICES);

	for (FVector& v : vertices)
	{
		v *= BlockSettings->BlockSize;
	}

	TArray<int32> triangles;
	triangles.Reserve(INDEX_COUNT);

	for (int i = 0; i < FACE_COUNT; ++i)
	{
		int32 idx = i * VERTICES_PER_FACE;

		triangles.Add(idx);
		triangles.Add(idx + 1);
		triangles.Add(idx + 2);

		triangles.Add(idx);
		triangles.Add(idx + 2);
		triangles.Add(idx + 3);
	}

	TArray<FVector2D> uvs;
	uvs.Append(UBlockSettings::TEST_UVS); //side
	uvs.Append(UBlockSettings::TEST_UVS); //side
	uvs.Append(UBlockSettings::TEST_UVS); //side
	uvs.Append(UBlockSettings::TEST_UVS); //side
	uvs.Append(UBlockSettings::TEST_UVS); //top
	uvs.Append(UBlockSettings::TEST_UVS); //bottom

	TArray<FVector> normals;
	TArray<FProcMeshTangent> tangents;
	TArray<FLinearColor> vertexColors;

	UKismetProceduralMeshLibrary::CalculateTangentsForMesh(vertices, triangles, uvs, normals, tangents);

	ProceduralMeshComponent->CreateMeshSection_LinearColor(0, vertices, triangles, normals, uvs, vertexColors, tangents, true);
	ProceduralMeshComponent->SetMaterial(0, Material);
}