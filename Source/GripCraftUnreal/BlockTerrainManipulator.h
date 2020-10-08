// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BlockSettings.h"
#include "BlockTerrainManipulator.generated.h"

UCLASS()
class GRIPCRAFTUNREAL_API ABlockTerrainManipulator : public AActor
{
	GENERATED_BODY()

public:	
	ABlockTerrainManipulator();

	void ChangeBlock(int direction);
	void PlaceCurrentBlock();
	void Update(FVector ViewOrigin, FVector ViewDirection);

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleDefaultsOnly)
	class UProceduralMeshComponent* ProceduralMeshComponent;

	UPROPERTY(EditDefaultsOnly)
	class UMaterialInterface* Material;

	UPROPERTY(EditDefaultsOnly)
	UBlockSettings* BlockSettings;

	UPROPERTY(EditAnywhere)
	TArray<EBlockType> BlockVariants;

	UPROPERTY(EditAnywhere)
	float MaxRaycastDistance = 10.0f;

	int SelectedBlockIndex = 0;
	EBlockType UVsSetForBlock = EBlockType::None;
	bool bPlaceCurrentBlock = false;
	TArray<FVector> Vertices;
	TArray<int32> Triangles;
	TArray<FVector2D> UVs;

	void CreateBlock();
	void UpdateUVs(EBlockType BlockType);
	void UpdateSelectedBlock(bool bVisible);
	void Show(bool bVisible) const;
	EBlockType GetSelectedBlock() const { return BlockVariants[SelectedBlockIndex]; }
};