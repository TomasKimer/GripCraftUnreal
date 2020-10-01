#pragma once

class BlockSettings
{
public:
    static const TArray<FVector> LEFT_VERTICES;
    static const TArray<FVector> RIGHT_VERTICES;
    static const TArray<FVector> FRONT_VERTICES;
    static const TArray<FVector> BACK_VERTICES;
    static const TArray<FVector> TOP_VERTICES;
    static const TArray<FVector> BOTTOM_VERTICES;

    static const float TILE_SIZE;
    static const TArray<FVector2D> TEST_UVS;
};
