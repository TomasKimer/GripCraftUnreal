// Fill out your copyright notice in the Description page of Project Settings.


#include "BlockSettings.h"


const TArray<FVector> UBlockSettings::LeftVertices = {
    FVector(0, 1, 0),
    FVector(0, 1, 1),
    FVector(0, 0, 1),
    FVector(0, 0, 0),
};

const TArray<FVector> UBlockSettings::RightVertices = {
    FVector(1, 0, 0),
    FVector(1, 0, 1),
    FVector(1, 1, 1),
    FVector(1, 1, 0),
};

const TArray<FVector> UBlockSettings::FrontVertices = {
    FVector(0, 0, 0),
    FVector(0, 0, 1),
    FVector(1, 0, 1),
    FVector(1, 0, 0),
};

const TArray<FVector> UBlockSettings::BackVertices = {
    FVector(1, 1, 0),
    FVector(1, 1, 1),
    FVector(0, 1, 1),
    FVector(0, 1, 0),
};

const TArray<FVector> UBlockSettings::TopVertices = {
    FVector(0, 0, 1),
    FVector(0, 1, 1),
    FVector(1, 1, 1),
    FVector(1, 0, 1),
};

const TArray<FVector> UBlockSettings::BottomVertices = {
    FVector(0, 0, 0),
    FVector(1, 0, 0),
    FVector(1, 1, 0),
    FVector(0, 1, 0),
};


TSharedPtr<UBlockSettings::FBlockInfoMap, ESPMode::ThreadSafe> UBlockSettings::GetBlockInfoMap()
{
    if (CachedBlockInfoMap != nullptr)
        return CachedBlockInfoMap;

//  CachedBlockInfoMap = MakeShared<FBlockInfoMap, ESPMode::ThreadSafe> { { .. }, .. } // [C3321] an initializer list is unexpected in this context
    CachedBlockInfoMap = MakeShareable(new FBlockInfoMap
    {
        { EBlockType::Grass, CreateBlockInfo(EBlockType::Grass) },
        { EBlockType::Dirt,  CreateBlockInfo(EBlockType::Dirt)  },
        { EBlockType::Snow,  CreateBlockInfo(EBlockType::Snow)  },
        { EBlockType::Stone, CreateBlockInfo(EBlockType::Stone) },
    });

    return CachedBlockInfoMap;
}

TSharedPtr<UBlockSettings::FBlockInfo, ESPMode::ThreadSafe> UBlockSettings::CreateBlockInfo(const EBlockType InBlockType) const
{
    const FBlockSetup* CurrBlockSetup = BlockSetup.FindByPredicate([InBlockType](const FBlockSetup& Obj) { return Obj.BlockType == InBlockType; });
    check(CurrBlockSetup != nullptr);

    return MakeShared<FBlockInfo, ESPMode::ThreadSafe>
    (
        GetUVs(CurrBlockSetup->TileTop),
        GetUVs(CurrBlockSetup->TileSide),
        GetUVs(CurrBlockSetup->TileBottom),
        CurrBlockSetup->Health        
    );
}

TArray<FVector2D> UBlockSettings::GetUVs(const ETile InTile) const
{
    const FTileSetup* CurrTileSetup = TileSetup.FindByPredicate([InTile](const FTileSetup& Obj) { return Obj.Tile == InTile; });
    check(CurrTileSetup != nullptr);

    const FIntPoint& Position = CurrTileSetup->Position;

    return TArray<FVector2D>
    {
        FVector2D((Position.X + 0.0f) / TileSize, (Position.Y + 1.0f) / TileSize),
        FVector2D((Position.X + 0.0f) / TileSize, (Position.Y + 0.0f) / TileSize),
        FVector2D((Position.X + 1.0f) / TileSize, (Position.Y + 0.0f) / TileSize),
        FVector2D((Position.X + 1.0f) / TileSize, (Position.Y + 1.0f) / TileSize),
    };
}
