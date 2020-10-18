#pragma once

#include "BlockSettings.h"

struct FBlockData
{
	EBlockType BlockType;
	float Health;

	friend FArchive& operator<<(FArchive& Ar, FBlockData& BlockData)
	{
		Ar << BlockData.BlockType;
		Ar << BlockData.Health;

		return Ar;
	}
};
