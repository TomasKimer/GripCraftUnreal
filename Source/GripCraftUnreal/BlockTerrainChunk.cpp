// Fill out your copyright notice in the Description page of Project Settings.


#include "BlockTerrainChunk.h"
#include "ProceduralMeshComponent.h"
#include "KismetProceduralMeshLibrary.h"
#include "BlockSettings.h"

// Sets default values
ABlockTerrainChunk::ABlockTerrainChunk()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// any of following returns null, why?
//	ProceduralMeshComp = Cast<UProceduralMeshComponent>(GetComponentByClass(UProceduralMeshComponent::StaticClass()));
//	ProceduralMeshComp = FindComponentByClass<UProceduralMeshComponent>()

	// used this instead
	ProceduralMeshComp = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProcMeshComp"));
	check(ProceduralMeshComp != nullptr);

	NoiseLib.SetNoiseType(FastNoiseLite::NoiseType_Perlin);

	BlockData = new TArray3D<bool>(Width, Width, Height);
}

// Called when the game starts or when spawned
void ABlockTerrainChunk::BeginPlay()
{
	Super::BeginPlay();

	GenerateHeightmap(0, 0);
	UpdateMesh();
	//CreateTestCube();
}

// Called every frame
void ABlockTerrainChunk::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABlockTerrainChunk::GenerateHeightmap(int PosX, int PosY)
{
	for (int x = 0; x < Width; ++x)
	{
		for (int y = 0; y < Width; ++y)
		{
			int height = GetTerrainHeight(PosX + x, PosY + y);

			for (int z = 0; z < height; ++z)
			{
				BlockData->Get(x, y, z) = true;
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
					AddFaceVertices(vertices, pos, BlockSettings::LEFT_VERTICES);
					uvs.Append(BlockSettings::TEST_UVS);
					faceCount += 1;
				}

				if (IsNone(x + 1, y, z))
				{
					AddFaceVertices(vertices, pos, BlockSettings::RIGHT_VERTICES);
					uvs.Append(BlockSettings::TEST_UVS);
					faceCount += 1;
				}

				if (IsNone(x, y, z - 1))
				{
					AddFaceVertices(vertices, pos, BlockSettings::BOTTOM_VERTICES);
					uvs.Append(BlockSettings::TEST_UVS);
					faceCount += 1;
				}

				if (IsNone(x, y, z + 1))
				{
					AddFaceVertices(vertices, pos, BlockSettings::TOP_VERTICES);
					uvs.Append(BlockSettings::TEST_UVS);
					faceCount += 1;
				}

				if (IsNone(x, y + 1, z))
				{
					AddFaceVertices(vertices, pos, BlockSettings::BACK_VERTICES);
					uvs.Append(BlockSettings::TEST_UVS);
					faceCount += 1;
				}

				if (IsNone(x, y - 1, z))
				{
					AddFaceVertices(vertices, pos, BlockSettings::FRONT_VERTICES);
					uvs.Append(BlockSettings::TEST_UVS);
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

	// TODO looks weird, something must be wrong here
//	UKismetProceduralMeshLibrary::CalculateTangentsForMesh(vertices, triangles, uvs, normals, tangents);

	ProceduralMeshComp->CreateMeshSection_LinearColor(0, vertices, triangles, normals, uvs, vertexColors, tangents, true);
	ProceduralMeshComp->SetMaterial(0, Material);
}

int ABlockTerrainChunk::GetTerrainHeight(int X, int Y)
{
	float perlinCoordX = X * PerlinScale + PerlinOffset.X;
	float perlinCoordY = Y * PerlinScale + PerlinOffset.Y;
	float perlinSample = NoiseLib.GetNoise(perlinCoordX, perlinCoordY);

	int result = FMath::Clamp(FMath::FloorToInt(perlinSample * Height), 0, Height - 1);
	
//	UE_LOG(LogTemp, Display, TEXT("X:%f Y:%f Sample:%f Result:%d"), perlinCoordX, perlinCoordY, perlinSample, result);
	
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

void ABlockTerrainChunk::AddFaceVertices(TArray<FVector>& Vertices, const FVector& Origin, const TArray<FVector>& VerticesToAdd)
{
	const int CUBE_SIZE_IN_CM = 100;
	
	for (int idx = 0; idx < VerticesToAdd.Num(); ++idx)
	{
		Vertices.Add((Origin + VerticesToAdd[idx]) * CUBE_SIZE_IN_CM);
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
	const int   CUBE_SIZE_IN_CM   = 100;

	TArray<FVector> vertices;
	vertices.Reserve(VERTEX_COUNT);

	vertices.Append(BlockSettings::LEFT_VERTICES);
	vertices.Append(BlockSettings::RIGHT_VERTICES);
	vertices.Append(BlockSettings::FRONT_VERTICES);
	vertices.Append(BlockSettings::BACK_VERTICES);
	vertices.Append(BlockSettings::TOP_VERTICES);
	vertices.Append(BlockSettings::BOTTOM_VERTICES);

	for (FVector& v : vertices)
	{
		v *= CUBE_SIZE_IN_CM;
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
	uvs.Append(BlockSettings::TEST_UVS); //side
	uvs.Append(BlockSettings::TEST_UVS); //side
	uvs.Append(BlockSettings::TEST_UVS); //side
	uvs.Append(BlockSettings::TEST_UVS); //side
	uvs.Append(BlockSettings::TEST_UVS); //top
	uvs.Append(BlockSettings::TEST_UVS); //bottom
	
	TArray<FVector> normals;
	TArray<FProcMeshTangent> tangents;
	TArray<FLinearColor> vertexColors;

	UKismetProceduralMeshLibrary::CalculateTangentsForMesh(vertices, triangles, uvs, normals, tangents);

	ProceduralMeshComp->CreateMeshSection_LinearColor(0, vertices, triangles, normals, uvs, vertexColors, tangents, true);
	ProceduralMeshComp->SetMaterial(0, Material);
}