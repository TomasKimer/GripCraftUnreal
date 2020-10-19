#pragma once

#include "Array3D.h"
#include "BlockSettings.h"
#include "KismetProceduralMeshLibrary.h"

struct FBlockData;

class FBlockTerrainMeshDataTask final : public FNonAbandonableTask
{
	friend class FAsyncTask<FBlockTerrainMeshDataTask>;

public:
	// Output data
	TArray<FVector> Vertices;
	TArray<int32> Triangles;
	TArray<FVector2D> UVs;
	TArray<FVector> Normals;
	TArray<FProcMeshTangent> Tangents;
	TArray<FLinearColor> VertexColors;

private:
	FBlockTerrainMeshDataTask(int InWidth, int InHeight, int InBlockSize, const TArray3D<FBlockData>* InBlockData,
	                          const TSharedPtr<UBlockSettings::FBlockInfoMap, ESPMode::ThreadSafe> InBlockInfoMap)
	: Width(InWidth)
	, Height(InHeight)
	, BlockSize(InBlockSize)
	, BlockData(InBlockData)
	, BlockInfoMap(InBlockInfoMap)
	{ }

	// Input data
	int Width;
	int Height;
	int BlockSize;
	const TArray3D<FBlockData>* BlockData;
	const TSharedPtr<UBlockSettings::FBlockInfoMap, ESPMode::ThreadSafe> BlockInfoMap;

	void DoWork();
	FORCEINLINE TStatId GetStatId() const { RETURN_QUICK_DECLARE_CYCLE_STAT(FBlockTerrainMeshTask, STATGROUP_ThreadPoolAsyncTasks); }

	void AddFaceVertices(const FVector& Origin, const TArray<FVector>& VerticesToAdd);
	bool IsNone(int X, int Y, int Z) const;
	bool CheckBounds(int X, int Y, int Z) const;
};
