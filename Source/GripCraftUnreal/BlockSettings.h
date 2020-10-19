// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BlockType.h"
#include "BlockSettings.generated.h"


UENUM()
enum class ETile : uint8
{
	Grass,
	GrassSide,
	Stone,
	Dirt,
	Snow,
	SnowSide
};


USTRUCT()
struct FBlockSetup
{
	GENERATED_BODY()
 
	UPROPERTY(EditAnywhere)
	EBlockType BlockType = EBlockType::Grass;

	UPROPERTY(EditAnywhere)
	ETile TileTop = ETile::Grass;

	UPROPERTY(EditAnywhere)
	ETile TileSide = ETile::Grass;

	UPROPERTY(EditAnywhere)
	ETile TileBottom = ETile::Grass;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "1", ClampMax = "1000"))
	float Health = 100.0f;
};


USTRUCT()
struct FTileSetup
{
	GENERATED_BODY()
 
    UPROPERTY(EditAnywhere)
	ETile Tile = ETile::Grass;

	UPROPERTY(EditAnywhere)
	FIntPoint Position;
};


UCLASS()
class GRIPCRAFTUNREAL_API UBlockSettings final : public UDataAsset
{
	GENERATED_BODY()

public:
	struct FBlockInfo
	{
		TArray<FVector2D> TopUVs;
		TArray<FVector2D> SideUVs;
		TArray<FVector2D> BottomUVs;
		float             Health;

		FBlockInfo(const TArray<FVector2D>&& InTopUVs, const TArray<FVector2D>&& InSideUVs,
		           const TArray<FVector2D>&& InBottomUVs, const float InHealth)
		{
			TopUVs = InTopUVs;
			SideUVs = InSideUVs;
			BottomUVs = InBottomUVs;
			Health = InHealth;
		}
	};

	typedef TMap<EBlockType, TSharedPtr<FBlockInfo, ESPMode::ThreadSafe>> FBlockInfoMap;

	static const TArray<FVector> LeftVertices;
	static const TArray<FVector> RightVertices;
	static const TArray<FVector> FrontVertices;
	static const TArray<FVector> BackVertices;
	static const TArray<FVector> TopVertices;
	static const TArray<FVector> BottomVertices;

	UPROPERTY(EditAnywhere, Category = "Blocks", meta = (ClampMin = "10", ClampMax = "1000", DisplayName="BlockSize [cm]"))
	int BlockSize = 100;

	TSharedPtr<FBlockInfoMap, ESPMode::ThreadSafe> GetBlockInfoMap();

private:
	UPROPERTY(EditAnywhere, Category = "Blocks")
	TArray<FBlockSetup> BlockSetup;

	UPROPERTY(EditAnywhere, Category = "Tiles", meta = (ClampMin = "1", ClampMax = "100"))
	int TileSize = 16;

	UPROPERTY(EditAnywhere, Category = "Tiles")
	TArray<FTileSetup> TileSetup;

	TSharedPtr<FBlockInfoMap, ESPMode::ThreadSafe> CachedBlockInfoMap;

	TSharedPtr<FBlockInfo, ESPMode::ThreadSafe> CreateBlockInfo(const EBlockType InBlockType) const;
	TArray<FVector2D> GetUVs(const ETile InTile) const;
};
