// Fill out your copyright notice in the Description page of Project Settings.


#include "BlockTerrainChunk.h"
#include "Array3D.h"
#include "BlockData.h"
#include "BlockTerrainMeshDataTask.h"
#include "BlockSettings.h"
#include "FastNoiseLite.h"
#include "ProceduralMeshComponent.h"
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

void ABlockTerrainChunk::Initialize(int InWidth, int InHeight, UBlockSettings* InBlockSettings, TArray3D<FBlockData>* InBlockData)
{
	Width = InWidth;
	Height = InHeight;
	BlockSettings = InBlockSettings;

	if (BlockData != nullptr)
		delete BlockData;

	if (InBlockData == nullptr)
	{
		BlockData = new TArray3D<FBlockData>(Width, Width, Height);
		bHasChanged = false;
	}
	else
	{
		BlockData = InBlockData;
		bHasChanged = true;
	}
}

void ABlockTerrainChunk::DeInitialize()
{
	ProceduralMeshComponent->SetMeshSectionVisible(0, false);

	TryCancelMeshDataTask();
}

void ABlockTerrainChunk::GenerateHeightmap(int PosX, int PosY, float NoiseScale, const FVector2D NoiseOffset, FastNoiseLite& NoiseLib) const
{
	const auto& BlockInfoMap = *BlockSettings->GetBlockInfoMap();

	for (int x = 0; x < Width; ++x)
	{
		for (int y = 0; y < Width; ++y)
		{
			int CurrentHeight = GetTerrainHeight(PosX + x, PosY + y, NoiseScale, NoiseOffset, NoiseLib);

			for (int z = 0; z < Height; ++z)
			{
				const EBlockType CurrentBlockType = GetBlockTypeForHeight(z, CurrentHeight);
				FBlockData& CurrentBlockData = BlockData->Get(x, y, z);

				CurrentBlockData.BlockType = CurrentBlockType;

				if (CurrentBlockType != EBlockType::None)
				{
					CurrentBlockData.Health = BlockInfoMap[CurrentBlockType]->Health;
				}
			}
		}
	}
}

void ABlockTerrainChunk::SetBlock(int X, int Y, int Z, const EBlockType BlockType)
{
	if (CheckBounds(X, Y, Z) == false)
		return;

	FBlockData& CurrentBlockData = BlockData->Get(X, Y, Z);

	if (CurrentBlockData.BlockType != EBlockType::None)
	{
		UE_LOG(LogTemp, Warning, TEXT("Block already occupied"));
	}

	CurrentBlockData.BlockType = BlockType;
	CurrentBlockData.Health    = (*BlockSettings->GetBlockInfoMap())[BlockType]->Health;

	UpdateMesh();

	bHasChanged = true;
}

void ABlockTerrainChunk::DamageBlock(int X, int Y, int Z, float Damage)
{
	if (CheckBounds(X, Y, Z) == false)
		return;
	if (Z == 0)
		return;

	FBlockData& CurrentBlockData = BlockData->Get(X, Y, Z);

	if (CurrentBlockData.BlockType == EBlockType::None)
	{
		UE_LOG(LogTemp, Warning, TEXT("Block already destroyed"));
		return;
	}

	float Health = CurrentBlockData.Health;
	Health -= Damage;

	if (Health <= 0.0f)
	{
		CurrentBlockData.BlockType = EBlockType::None;
		UpdateMesh();
	}

	CurrentBlockData.Health = FMath::Max(0.0f, Health);

	bHasChanged = true;
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
	TArray3D<FBlockData>* OutData = BlockData;
	BlockData = nullptr;

	return OutData;
}

void ABlockTerrainChunk::TryFinishMeshDataTask()
{
	if (MeshDataTask == nullptr)
		return;
	if (MeshDataTask->IsDone() == false)
		return;

	const FBlockTerrainMeshDataTask& CompletedTask = MeshDataTask->GetTask();

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

int ABlockTerrainChunk::GetTerrainHeight(int X, int Y, float NoiseScale, const FVector2D NoiseOffset, FastNoiseLite& NoiseLib) const
{
	float NoiseCoordX = X * NoiseScale + NoiseOffset.X;
	float NoiseCoordY = Y * NoiseScale + NoiseOffset.Y;
	float NoiseSample = (NoiseLib.GetNoise(NoiseCoordX, NoiseCoordY) * 0.5f) + 0.5f;

	int Result = FMath::Clamp(FMath::FloorToInt(NoiseSample * Height), 0, Height - 1);

//	UE_LOG(LogTemp, Display, TEXT("X:%f Y:%f Sample:%f Result:%d"), NoiseCoordX, NoiseCoordY, NoiseSample, Result);

	return Result;
}

EBlockType ABlockTerrainChunk::GetBlockTypeForHeight(int InHeight, int CurrMaxHeight) const
{
	if (InHeight > CurrMaxHeight)
		return EBlockType::None;

	float Fraction = InHeight / static_cast<float>(Height);

	if (Fraction < 0.2f)
		return EBlockType::Stone;
	if (Fraction < 0.4f)
		return EBlockType::Dirt;
	if (Fraction < 0.6f)
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
