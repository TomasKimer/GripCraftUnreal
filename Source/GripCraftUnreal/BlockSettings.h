// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BlockSettings.generated.h"


UENUM()
enum class EBlockType : uint8
{
	None,
	Grass,
	Dirt,
	Snow,
	Stone
};


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
	GENERATED_USTRUCT_BODY()
 
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
	GENERATED_USTRUCT_BODY()
 
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

		FBlockInfo(TArray<FVector2D> topUVs, TArray<FVector2D> sideUVs, TArray<FVector2D> bottomUVs, float health)
		{
			TopUVs = topUVs;
			SideUVs = sideUVs;
			BottomUVs = bottomUVs;
			Health = health;
		}
	};

	static const TArray<FVector> LEFT_VERTICES;
	static const TArray<FVector> RIGHT_VERTICES;
	static const TArray<FVector> FRONT_VERTICES;
	static const TArray<FVector> BACK_VERTICES;
	static const TArray<FVector> TOP_VERTICES;
	static const TArray<FVector> BOTTOM_VERTICES;

	UPROPERTY(EditAnywhere, Category = "Blocks", meta = (ClampMin = "10", ClampMax = "1000", DisplayName="BlockSize [cm]"))
	int BlockSize = 100;

	TSharedPtr<FBlockInfo> GetBlockInfo(EBlockType blockType);

private:
	UPROPERTY(EditAnywhere, Category = "Blocks")
	TArray<FBlockSetup> BlockSetup;

	UPROPERTY(EditAnywhere, Category = "Tiles", meta = (ClampMin = "1", ClampMax = "100"))
	int TileSize = 16;

	UPROPERTY(EditAnywhere, Category = "Tiles")
	TArray<FTileSetup> TileSetup;

	TMap<EBlockType, TSharedPtr<FBlockInfo>> CachedBlockInfos;

	TArray<FVector2D> GetUVs(ETile tile) const;
};
