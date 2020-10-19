
#include "BlockTerrainMeshDataTask.h"
#include "BlockData.h"
#include "BlockSettings.h"
//#include "HAL/ThreadManager.h"

void FBlockTerrainMeshDataTask::DoWork()
{
//	double StartTime = FPlatformTime::Seconds();
//	UE_LOG(LogTemp, Display, TEXT(":::: TASK STARTED Thread=%s Frame=%d"), *FThreadManager::Get().GetThreadName(FPlatformTLS::GetCurrentThreadId()), GFrameCounter);

	static const int VERTICES_PER_FACE = 4;

	for (int x = 0; x < Width; ++x)
	{
		for (int y = 0; y < Width; ++y)
		{
			for (int z = 0; z < Height; ++z)
			{
				const EBlockType BlockType = BlockData->Get(x, y, z).BlockType;
				if (BlockType == EBlockType::None)
					continue;

				const auto& BlockInfo = (*BlockInfoMap)[BlockType];
				int FaceCount = 0;
				FVector Pos(x, y, z);

				if (IsNone(x - 1, y, z))
				{
					AddFaceVertices(Pos, UBlockSettings::LEFT_VERTICES);
					UVs.Append(BlockInfo->SideUVs);
					FaceCount += 1;
				}

				if (IsNone(x + 1, y, z))
				{
					AddFaceVertices(Pos, UBlockSettings::RIGHT_VERTICES);
					UVs.Append(BlockInfo->SideUVs);
					FaceCount += 1;
				}

				if (IsNone(x, y, z - 1))
				{
					AddFaceVertices(Pos, UBlockSettings::BOTTOM_VERTICES);
					UVs.Append(BlockInfo->BottomUVs);
					FaceCount += 1;
				}

				if (IsNone(x, y, z + 1))
				{
					AddFaceVertices(Pos, UBlockSettings::TOP_VERTICES);
					UVs.Append(BlockInfo->TopUVs);
					FaceCount += 1;
				}

				if (IsNone(x, y + 1, z))
				{
					AddFaceVertices(Pos, UBlockSettings::BACK_VERTICES);
					UVs.Append(BlockInfo->SideUVs);
					FaceCount += 1;
				}

				if (IsNone(x, y - 1, z))
				{
					AddFaceVertices(Pos, UBlockSettings::FRONT_VERTICES);
					UVs.Append(BlockInfo->SideUVs);
					FaceCount += 1;
				}

				int StartIdx = Vertices.Num() - FaceCount * VERTICES_PER_FACE;
				for (int i = 0; i < FaceCount; ++i)
				{
					int Idx = StartIdx + i * VERTICES_PER_FACE;

					Triangles.Add(Idx);
					Triangles.Add(Idx + 1);
					Triangles.Add(Idx + 2);

					Triangles.Add(Idx);
					Triangles.Add(Idx + 2);
					Triangles.Add(Idx + 3);
				}
			}
		}
	}

	// TODO there is something wrong with the normal computation, it looks better without it
//	UKismetProceduralMeshLibrary::CalculateTangentsForMesh(Vertices, Triangles, UVs, Normals, Tangents);

//	UE_LOG(LogTemp, Display, TEXT(":::: TASK FINISHED %d, %f"), GFrameCounter, FPlatformTime::Seconds() - StartTime);
}

void FBlockTerrainMeshDataTask::AddFaceVertices(const FVector& Origin, const TArray<FVector>& VerticesToAdd)
{
	for (int Idx = 0; Idx < VerticesToAdd.Num(); ++Idx)
	{
		Vertices.Add((Origin + VerticesToAdd[Idx]) * BlockSize);
	}
}

bool FBlockTerrainMeshDataTask::IsNone(int X, int Y, int Z) const
{
	if (CheckBounds(X, Y, Z) == false)
		return true;

	return BlockData->Get(X, Y, Z).BlockType == EBlockType::None;
}

bool FBlockTerrainMeshDataTask::CheckBounds(int X, int Y, int Z) const
{
	if (X < 0 || Y < 0 || Z < 0)
		return false;
	if (X >= Width || Y >= Width || Z >= Height)
		return false;

	return true;
}
