// Fill out your copyright notice in the Description page of Project Settings.


#include "BlockTerrainManipulator.h"
#include "BlockTerrainChunk.h"
#include "DrawDebugHelpers.h"
#include "ProceduralMeshComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "BlockTerrainManager.h"
#include "Chaos/PBDCollisionConstraintsContact.h"


ABlockTerrainManipulator::ABlockTerrainManipulator()
{
	PrimaryActorTick.bCanEverTick = true;

	ProceduralMeshComponent = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProceduralMeshComponent"));
	RootComponent = ProceduralMeshComponent;
}

void ABlockTerrainManipulator::BeginPlay()
{
	Super::BeginPlay();

	CreateBlock();
	UpdateSelectedBlock(true);
}

void ABlockTerrainManipulator::ChangeBlock(int direction)
{
	if (direction == 0)
		return;

	direction = FMath::Clamp(direction, -1, 1);

	SelectedBlockIndex += direction;

	if (SelectedBlockIndex < 0)
	{
		SelectedBlockIndex = BlockVariants.Num() - 1;
	}
	else if (SelectedBlockIndex >= BlockVariants.Num())
	{
		SelectedBlockIndex = 0;
	}
}

void ABlockTerrainManipulator::PlaceCurrentBlock()
{
	bPlaceCurrentBlock = true;
}

void ABlockTerrainManipulator::Update(FVector ViewOrigin, FVector ViewDirection)
{
	EBlockType selectedBlock = GetSelectedBlock();
	if (selectedBlock == EBlockType::None)
	{
		UpdateSelectedBlock(false);
		bPlaceCurrentBlock = false;
		return;
	}

	const FCollisionQueryParams params;
	FHitResult hitResult;
	const int blockSize = BlockSettings->BlockSize;
	const FVector endLocation = ViewOrigin + ViewDirection * MaxRaycastDistance * blockSize;

	if (GetWorld()->LineTraceSingleByChannel(hitResult, ViewOrigin, endLocation, ECC_GameTraceChannel2, params) == true)
	{
		const FVector hitBlockPosition = hitResult.Location + hitResult.Normal * blockSize * 0.5f;

//		DrawDebugLine(GetWorld(), hitResult.Location, hitBlockPosition, FColor(0, 255, 0));

		FVector alignedBlockLocation(
			blockSize * FMath::FloorToInt(hitBlockPosition.X / blockSize),
			blockSize * FMath::FloorToInt(hitBlockPosition.Y / blockSize),
			blockSize * FMath::FloorToInt(hitBlockPosition.Z / blockSize)
		);

		SetActorLocation(alignedBlockLocation);
		UpdateSelectedBlock(true);

		if (bPlaceCurrentBlock == true)
		{
			const AActor* hitActor = hitResult.Actor.Get(); // a weak pointer to the Actor that the trace hit
			ABlockTerrainManager* blockTerrainManager = Cast<ABlockTerrainManager>(hitActor->GetAttachParentActor());

			if (blockTerrainManager != nullptr)
			{
				blockTerrainManager->AddBlock(hitBlockPosition, selectedBlock);
			}
		}
	}
	else
	{
		UpdateSelectedBlock(false);
	}

	bPlaceCurrentBlock = false;
}

void ABlockTerrainManipulator::CreateBlock()
{
	const int FACE_COUNT        = 6;
	const int INDICES_PER_FACE  = 6;
	const int VERTICES_PER_FACE = 4;
	const int VERTEX_COUNT      = FACE_COUNT * VERTICES_PER_FACE;
	const int INDEX_COUNT       = FACE_COUNT * INDICES_PER_FACE;

	Vertices.Reserve(VERTEX_COUNT);
	Vertices.Append(UBlockSettings::LEFT_VERTICES);
	Vertices.Append(UBlockSettings::RIGHT_VERTICES);
	Vertices.Append(UBlockSettings::FRONT_VERTICES);
	Vertices.Append(UBlockSettings::BACK_VERTICES);
	Vertices.Append(UBlockSettings::TOP_VERTICES);
	Vertices.Append(UBlockSettings::BOTTOM_VERTICES);

	for (FVector& v : Vertices)
	{
		v *= BlockSettings->BlockSize;
	}

	Triangles.Reserve(INDEX_COUNT);

	for (int i = 0; i < FACE_COUNT; ++i)
	{
		int32 idx = i * VERTICES_PER_FACE;

		Triangles.Add(idx);
		Triangles.Add(idx + 1);
		Triangles.Add(idx + 2);

		Triangles.Add(idx);
		Triangles.Add(idx + 2);
		Triangles.Add(idx + 3);
	}

	TArray<FVector> normals;
	TArray<FProcMeshTangent> tangents;
	TArray<FLinearColor> vertexColors;

	ProceduralMeshComponent->CreateMeshSection_LinearColor(0, Vertices, Triangles, normals, UVs, vertexColors, tangents, false);
	ProceduralMeshComponent->SetMaterial(0, Material);
}

void ABlockTerrainManipulator::UpdateUVs(EBlockType BlockType)
{
	if (UVsSetForBlock == BlockType)
		return;
	UVsSetForBlock = BlockType;

	TArray<FVector> normals;
	TArray<FProcMeshTangent> tangents;
	TArray<FLinearColor> vertexColors;

	const TSharedPtr<UBlockSettings::FBlockInfo> blockInfo = BlockSettings->GetBlockInfo(BlockType);

	UVs.Append(blockInfo->SideUVs);
	UVs.Append(blockInfo->SideUVs);
	UVs.Append(blockInfo->SideUVs);
	UVs.Append(blockInfo->SideUVs);
	UVs.Append(blockInfo->TopUVs);
	UVs.Append(blockInfo->BottomUVs);

	ProceduralMeshComponent->UpdateMeshSection_LinearColor(0, Vertices, normals, UVs, vertexColors, tangents);

	UVs.Empty();
}

void ABlockTerrainManipulator::UpdateSelectedBlock(bool bVisible)
{
	EBlockType selectedBlock = GetSelectedBlock();
	if (selectedBlock == EBlockType::None)
	{
		Show(false);
	}
	else
	{
		UpdateUVs(selectedBlock);
		Show(bVisible);
	}
}

void ABlockTerrainManipulator::Show(bool bVisible) const
{
	if (ProceduralMeshComponent->IsMeshSectionVisible(0) == bVisible)
		return;

	ProceduralMeshComponent->SetMeshSectionVisible(0, bVisible);
}
