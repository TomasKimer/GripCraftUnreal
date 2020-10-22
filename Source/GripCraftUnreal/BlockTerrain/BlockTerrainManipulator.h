// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BlockType.h"
#include "BlockTerrainManipulator.generated.h"

class UBlockSettings;

UCLASS()
class GRIPCRAFTUNREAL_API ABlockTerrainManipulator final : public AActor
{
	GENERATED_BODY()

public:	
	ABlockTerrainManipulator();

	void ChangeBlock(int Direction);
	void PlaceCurrentBlock();
	void Update(const FVector& ViewOrigin, const FVector& ViewDirection);

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
	void UpdateUVs(const EBlockType BlockType);
	void UpdateSelectedBlock(bool bVisible);
	void Show(bool bNewVisibility) const;
	FORCEINLINE EBlockType GetSelectedBlock() const { return BlockVariants[SelectedBlockIndex]; }
};
