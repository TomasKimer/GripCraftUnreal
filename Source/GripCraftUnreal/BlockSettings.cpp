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

const float UBlockSettings::TILE_SIZE = 16.0f;
const TArray<FVector2D> UBlockSettings::TEST_UVS = {
    //side
    FVector2D(0.0f / TILE_SIZE, 0.0f / TILE_SIZE),
    FVector2D(0.0f / TILE_SIZE, 1.0f / TILE_SIZE),
    FVector2D(1.0f / TILE_SIZE, 1.0f / TILE_SIZE),
    FVector2D(1.0f / TILE_SIZE, 0.0f / TILE_SIZE),
};

