// Fill out your copyright notice in the Description page of Project Settings.


#include "BlockSettings.h"


const TArray<FVector> UBlockSettings::LEFT_VERTICES = {
    FVector(0, 1, 0),
    FVector(0, 1, 1),
    FVector(0, 0, 1),
    FVector(0, 0, 0),
};

const TArray<FVector> UBlockSettings::RIGHT_VERTICES = {
    FVector(1, 0, 0),
    FVector(1, 0, 1),
    FVector(1, 1, 1),
    FVector(1, 1, 0),
};

const TArray<FVector> UBlockSettings::FRONT_VERTICES = {
    FVector(0, 0, 0),
    FVector(0, 0, 1),
    FVector(1, 0, 1),
    FVector(1, 0, 0),
};

const TArray<FVector> UBlockSettings::BACK_VERTICES = {
    FVector(1, 1, 0),
    FVector(1, 1, 1),
    FVector(0, 1, 1),
    FVector(0, 1, 0),
};

const TArray<FVector> UBlockSettings::TOP_VERTICES = {
    FVector(0, 0, 1),
    FVector(0, 1, 1),
    FVector(1, 1, 1),
    FVector(1, 0, 1),
};

const TArray<FVector> UBlockSettings::BOTTOM_VERTICES = {
    FVector(0, 0, 0),
    FVector(1, 0, 0),
    FVector(1, 1, 0),
    FVector(0, 1, 0),
};


TSharedPtr<UBlockSettings::FBlockInfo> UBlockSettings::GetBlockInfo(EBlockType blockType)
{
    TSharedPtr<FBlockInfo>* blockInfo = CachedBlockInfos.Find(blockType);
    if (blockInfo != nullptr)
        return *blockInfo;

    const FBlockSetup* blockSetup = BlockSetup.FindByPredicate([blockType](const FBlockSetup& obj) { return obj.BlockType == blockType; });
    check(blockSetup != nullptr);

    TSharedPtr<FBlockInfo> newBlockInfo = MakeShareable(new FBlockInfo
    (
        GetUVs(blockSetup->TileTop),
        GetUVs(blockSetup->TileSide),
        GetUVs(blockSetup->TileBottom),
        blockSetup->Health        
    ));

    CachedBlockInfos.Add(blockType, newBlockInfo);
    
    return newBlockInfo;
}


TArray<FVector2D> UBlockSettings::GetUVs(ETile tile) const
{
    const FTileSetup* tileSetup = TileSetup.FindByPredicate([tile](const FTileSetup& obj) { return obj.Tile == tile; });
    check(tileSetup != nullptr);

    FIntPoint position = tileSetup->Position;

    return TArray<FVector2D>
    {
        FVector2D((position.X + 0.0f) / TileSize, (position.Y + 1.0f) / TileSize),
        FVector2D((position.X + 0.0f) / TileSize, (position.Y + 0.0f) / TileSize),
        FVector2D((position.X + 1.0f) / TileSize, (position.Y + 0.0f) / TileSize),
        FVector2D((position.X + 1.0f) / TileSize, (position.Y + 1.0f) / TileSize),
    };
}
