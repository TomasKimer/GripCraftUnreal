#include "BlockSettings.h"

const TArray<FVector> BlockSettings::LEFT_VERTICES = {
    FVector(0, 1, 0),
    FVector(0, 1, 1),
    FVector(0, 0, 1),
    FVector(0, 0, 0),
};

const TArray<FVector> BlockSettings::RIGHT_VERTICES = {
    FVector(1, 0, 0),
    FVector(1, 0, 1),
    FVector(1, 1, 1),
    FVector(1, 1, 0),
};

const TArray<FVector> BlockSettings::FRONT_VERTICES = {
    FVector(0, 0, 0),
    FVector(0, 0, 1),
    FVector(1, 0, 1),
    FVector(1, 0, 0),
};

const TArray<FVector> BlockSettings::BACK_VERTICES = {
    FVector(1, 1, 0),
    FVector(1, 1, 1),
    FVector(0, 1, 1),
    FVector(0, 1, 0),
};

const TArray<FVector> BlockSettings::TOP_VERTICES = {
    FVector(0, 0, 1),
    FVector(0, 1, 1),
    FVector(1, 1, 1),
    FVector(1, 0, 1),
};

const TArray<FVector> BlockSettings::BOTTOM_VERTICES = {
    FVector(0, 0, 0),
    FVector(1, 0, 0),
    FVector(1, 1, 0),
    FVector(0, 1, 0),
};

const float BlockSettings::TILE_SIZE = 16.0f;
const TArray<FVector2D> BlockSettings::TEST_UVS = {
    //side
    FVector2D(0.0f / TILE_SIZE, 0.0f / TILE_SIZE),
    FVector2D(0.0f / TILE_SIZE, 1.0f / TILE_SIZE),
    FVector2D(1.0f / TILE_SIZE, 1.0f / TILE_SIZE),
    FVector2D(1.0f / TILE_SIZE, 0.0f / TILE_SIZE),
};
