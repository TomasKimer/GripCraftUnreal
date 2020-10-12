// Fill out your copyright notice in the Description page of Project Settings.


#include "BlockTerrainChunk.h"
#include "ProceduralMeshComponent.h"
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

void ABlockTerrainChunk::Initialize(int width, int height, UBlockSettings* blockSettings, TArray3D<FBlockData>* blockData)
{
	Width = width;
	Height = height;
	BlockSettings = blockSettings;

	if (BlockData != nullptr)
		delete BlockData;

	if (blockData == nullptr)
	{
		BlockData = new TArray3D<FBlockData>(Width, Width, Height);
		bChanged = false;
	}
	else
	{
		BlockData = blockData;
		bChanged = true;
	}
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
				EBlockType blockType = GetBlockTypeForHeight(z, height);

				BlockData->Get(x, y, z).BlockType = blockType;

				if (blockType != EBlockType::None)
				{
					BlockData->Get(x, y, z).Health = BlockSettings->GetBlockInfo(blockType)->Health;
				}
			}
		}
	}
}

void ABlockTerrainChunk::SetBlock(int X, int Y, int Z, EBlockType BlockType)
{
	if (CheckBounds(X, Y, Z) == false)
		return;

	if (BlockData->Get(X, Y, Z).BlockType != EBlockType::None)
	{
		UE_LOG(LogTemp, Warning, TEXT("Block already occupied"));
	}

	BlockData->Get(X, Y, Z).BlockType = BlockType;
	BlockData->Get(X, Y, Z).Health    = BlockSettings->GetBlockInfo(BlockType)->Health;

	UpdateMesh();

	bChanged = true;
}

void ABlockTerrainChunk::DamageBlock(int X, int Y, int Z, float Damage)
{
	if (CheckBounds(X, Y, Z) == false)
		return;
	if (Z == 0)
		return;

	if (BlockData->Get(X, Y, Z).BlockType == EBlockType::None)
	{
		UE_LOG(LogTemp, Warning, TEXT("Block already destroyed"));
		return;
	}

	float health = BlockData->Get(X, Y, Z).Health;
	health -= Damage;

	if (health <= 0.0f)
	{
		BlockData->Get(X, Y, Z).BlockType = EBlockType::None;
		UpdateMesh();
	}

	BlockData->Get(X, Y, Z).Health = FMath::Max(0.0f, health);

	bChanged = true;
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
				EBlockType blockType = BlockData->Get(x, y, z).BlockType;
				if (blockType == EBlockType::None)
					continue;

				const TSharedPtr<UBlockSettings::FBlockInfo> blockInfo = BlockSettings->GetBlockInfo(blockType);
				int faceCount = 0;
				FVector pos(x, y, z);

				if (IsNone(x - 1, y, z))
				{
					AddFaceVertices(vertices, pos, UBlockSettings::LEFT_VERTICES);
					uvs.Append(blockInfo->SideUVs);
					faceCount += 1;
				}

				if (IsNone(x + 1, y, z))
				{
					AddFaceVertices(vertices, pos, UBlockSettings::RIGHT_VERTICES);
					uvs.Append(blockInfo->SideUVs);
					faceCount += 1;
				}

				if (IsNone(x, y, z - 1))
				{
					AddFaceVertices(vertices, pos, UBlockSettings::BOTTOM_VERTICES);
					uvs.Append(blockInfo->BottomUVs);
					faceCount += 1;
				}

				if (IsNone(x, y, z + 1))
				{
					AddFaceVertices(vertices, pos, UBlockSettings::TOP_VERTICES);
					uvs.Append(blockInfo->TopUVs);
					faceCount += 1;
				}

				if (IsNone(x, y + 1, z))
				{
					AddFaceVertices(vertices, pos, UBlockSettings::BACK_VERTICES);
					uvs.Append(blockInfo->SideUVs);
					faceCount += 1;
				}

				if (IsNone(x, y - 1, z))
				{
					AddFaceVertices(vertices, pos, UBlockSettings::FRONT_VERTICES);
					uvs.Append(blockInfo->SideUVs);
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

	// TODO looks like this is not needed, it looks worse and is way toooo slow
//	UKismetProceduralMeshLibrary::CalculateTangentsForMesh(vertices, triangles, uvs, normals, tangents);

	ProceduralMeshComponent->ClearMeshSection(0);
	ProceduralMeshComponent->CreateMeshSection_LinearColor(0, vertices, triangles, normals, uvs, vertexColors, tangents, true);
	ProceduralMeshComponent->SetMaterial(0, Material);
}

TArray3D<FBlockData>* ABlockTerrainChunk::TakeBlockData()
{
	TArray3D<FBlockData>* data = BlockData;
	BlockData = nullptr;

	return data;
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

EBlockType ABlockTerrainChunk::GetBlockTypeForHeight(int InHeight, int CurrMaxHeight) const
{
	if (InHeight > CurrMaxHeight)
		return EBlockType::None;

	float fraction = InHeight / static_cast<float>(Height);

	if (fraction < 0.2f)
		return EBlockType::Stone;
	if (fraction < 0.4f)
		return EBlockType::Dirt;
	if (fraction < 0.6f)
		return EBlockType::Grass;

	return EBlockType::Snow;
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

	return BlockData->Get(X, Y, Z).BlockType == EBlockType::None;
}

bool ABlockTerrainChunk::CheckBounds(int X, int Y, int Z) const
{
	if (X < 0 || Y < 0 || Z < 0)
		return false;
	if (X >= Width || Y >= Width || Z >= Height)
		return false;

	return true;
}
