// Fill out your copyright notice in the Description page of Project Settings.


#include "BlockTerrainChunk.h"
#include "ProceduralMeshComponent.h"
#include "FastNoiseLite.h"
#include "BlockSettings.h"
#include "HAL/ThreadManager.h"
#include "Kismet/GameplayStatics.h"


ABlockTerrainChunk::ABlockTerrainChunk()
{
	PrimaryActorTick.bCanEverTick = true;

	ProceduralMeshComponent = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProceduralMeshComponent"));
	RootComponent = ProceduralMeshComponent;
}

ABlockTerrainChunk::~ABlockTerrainChunk()
{
	TryCancelMeshDataTask();

	delete BlockData;
}

void ABlockTerrainChunk::Tick(float DeltaSeconds)
{
	TryFinishMeshDataTask();
}

void ABlockTerrainChunk::Initialize(int InWidth, int InHeight, /*const*/ UBlockSettings* InBlockSettings, TArray3D<FBlockData>* InBlockData)
{
	Width = InWidth;
	Height = InHeight;
	BlockSettings = InBlockSettings;

	if (BlockData != nullptr)
		delete BlockData;

	if (InBlockData == nullptr)
	{
		BlockData = new TArray3D<FBlockData>(Width, Width, Height);
		bChanged = false;
	}
	else
	{
		BlockData = InBlockData;
		bChanged = true;
	}
}

void ABlockTerrainChunk::DeInitialize()
{
	ProceduralMeshComponent->SetMeshSectionVisible(0, false);

	TryCancelMeshDataTask();
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
					BlockData->Get(x, y, z).Health = (*BlockSettings->GetBlockInfoMap())[blockType]->Health;
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
	BlockData->Get(X, Y, Z).Health    = (*BlockSettings->GetBlockInfoMap())[BlockType]->Health;

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

void ABlockTerrainChunk::UpdateMesh()
{
	TryCancelMeshDataTask();

	MeshDataTask = new FAsyncTask<FBlockTerrainMeshDataTask>(Width, Height,  BlockSettings->BlockSize, BlockData, BlockSettings->GetBlockInfoMap());

	if (bUseAsyncGeneration == true)
	{
		MeshDataTask->StartBackgroundTask();
	}
	else
	{
		MeshDataTask->StartSynchronousTask();
	}
}

TArray3D<FBlockData>* ABlockTerrainChunk::TakeBlockData()
{
	TArray3D<FBlockData>* data = BlockData;
	BlockData = nullptr;

	return data;
}

void ABlockTerrainChunk::TryFinishMeshDataTask()
{
	if (MeshDataTask == nullptr)
		return;
	if (MeshDataTask->IsDone() == false)
		return;

	const FBlockTerrainMeshDataTask CompletedTask = MeshDataTask->GetTask();

	ProceduralMeshComponent->ClearMeshSection(0);
	ProceduralMeshComponent->CreateMeshSection_LinearColor(0, CompletedTask.Vertices, CompletedTask.Triangles, CompletedTask.Normals, CompletedTask.UVs, CompletedTask.VertexColors, CompletedTask.Tangents, true);
	ProceduralMeshComponent->SetMaterial(0, Material);
	ProceduralMeshComponent->SetMeshSectionVisible(0, true);

	delete MeshDataTask;
	MeshDataTask = nullptr;
}

void ABlockTerrainChunk::TryCancelMeshDataTask()
{
	if (MeshDataTask == nullptr)
		return;

	if (MeshDataTask->Cancel() == false)
	{
		MeshDataTask->EnsureCompletion(false);
	}

	delete MeshDataTask;
	MeshDataTask = nullptr;
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

bool ABlockTerrainChunk::CheckBounds(int X, int Y, int Z) const
{
	if (X < 0 || Y < 0 || Z < 0)
		return false;
	if (X >= Width || Y >= Width || Z >= Height)
		return false;

	return true;
}
