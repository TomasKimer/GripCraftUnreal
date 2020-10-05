// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BlockSettings.generated.h"


UCLASS()
class GRIPCRAFTUNREAL_API UBlockSettings : public UDataAsset
{
	GENERATED_BODY()

public:
	static const TArray<FVector> LEFT_VERTICES;
	static const TArray<FVector> RIGHT_VERTICES;
	static const TArray<FVector> FRONT_VERTICES;
	static const TArray<FVector> BACK_VERTICES;
	static const TArray<FVector> TOP_VERTICES;
	static const TArray<FVector> BOTTOM_VERTICES;

	static const float TILE_SIZE;
	static const TArray<FVector2D> TEST_UVS;

	UPROPERTY(EditAnywhere, meta = (ClampMin = "10", ClampMax = "1000"))
	int BlockSize = 100; // centimetres
};
