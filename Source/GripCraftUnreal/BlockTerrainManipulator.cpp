// Fill out your copyright notice in the Description page of Project Settings.


#include "BlockTerrainManipulator.h"
#include "BlockTerrainChunk.h"
#include "BlockSettings.h"
#include "ProceduralMeshComponent.h"
#include "BlockTerrainManager.h"
//#include "DrawDebugHelpers.h"


ABlockTerrainManipulator::ABlockTerrainManipulator()
{
	PrimaryActorTick.bCanEverTick = false;

	ProceduralMeshComponent = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProceduralMeshComponent"));
	RootComponent = ProceduralMeshComponent;
}

void ABlockTerrainManipulator::BeginPlay()
{
	Super::BeginPlay();

	CreateBlock();
	UpdateSelectedBlock(true);
}

void ABlockTerrainManipulator::ChangeBlock(int Direction)
{
	if (Direction == 0)
		return;

	Direction = FMath::Clamp(Direction, -1, 1);

	SelectedBlockIndex += Direction;

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

void ABlockTerrainManipulator::Update(const FVector& ViewOrigin, const FVector& ViewDirection)
{
	EBlockType SelectedBlock = GetSelectedBlock();
	if (SelectedBlock == EBlockType::None)
	{
		UpdateSelectedBlock(false);
		bPlaceCurrentBlock = false;
		return;
	}

	const FCollisionQueryParams Params;
	FHitResult HitResult;
	const int BlockSize = BlockSettings->BlockSize;
	const FVector& EndLocation = ViewOrigin + ViewDirection * MaxRaycastDistance * BlockSize;

	if (GetWorld()->LineTraceSingleByChannel(HitResult, ViewOrigin, EndLocation, ECC_GameTraceChannel2, Params) == true)
	{
		const FVector& HitBlockPosition = HitResult.Location + HitResult.Normal * BlockSize * 0.5f;

//		DrawDebugLine(GetWorld(), hitResult.Location, hitBlockPosition, FColor(0, 255, 0));

		FVector AlignedBlockLocation(
			BlockSize * FMath::FloorToInt(HitBlockPosition.X / BlockSize),
			BlockSize * FMath::FloorToInt(HitBlockPosition.Y / BlockSize),
			BlockSize * FMath::FloorToInt(HitBlockPosition.Z / BlockSize)
		);

		SetActorLocation(AlignedBlockLocation);
		UpdateSelectedBlock(true);

		if (bPlaceCurrentBlock == true)
		{
			ABlockTerrainManager* BlockTerrainManager = Cast<ABlockTerrainManager>(HitResult.Actor->GetAttachParentActor());

			if (BlockTerrainManager != nullptr)
			{
				BlockTerrainManager->AddBlock(HitBlockPosition, SelectedBlock);
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
	const int FaceCount       = 6;
	const int IndicesPerFace  = 6;
	const int VerticesPerFace = 4;
	const int VertexCount     = FaceCount * VerticesPerFace;
	const int IndexCount      = FaceCount * IndicesPerFace;

	Vertices.Reserve(VertexCount);
	Vertices.Append(UBlockSettings::LeftVertices);
	Vertices.Append(UBlockSettings::RightVertices);
	Vertices.Append(UBlockSettings::FrontVertices);
	Vertices.Append(UBlockSettings::BackVertices);
	Vertices.Append(UBlockSettings::TopVertices);
	Vertices.Append(UBlockSettings::BottomVertices);

	for (FVector& v : Vertices)
	{
		v *= BlockSettings->BlockSize;
	}

	Triangles.Reserve(IndexCount);

	for (int i = 0; i < FaceCount; ++i)
	{
		int32 Idx = i * VerticesPerFace;

		Triangles.Add(Idx);
		Triangles.Add(Idx + 1);
		Triangles.Add(Idx + 2);

		Triangles.Add(Idx);
		Triangles.Add(Idx + 2);
		Triangles.Add(Idx + 3);
	}

	const TArray<FVector> Normals;
	const TArray<FLinearColor> VertexColors;
	const TArray<FProcMeshTangent> Tangents;

	ProceduralMeshComponent->CreateMeshSection_LinearColor(0, Vertices, Triangles, Normals, UVs, VertexColors, Tangents, false);
	ProceduralMeshComponent->SetMaterial(0, Material);
}

void ABlockTerrainManipulator::UpdateUVs(const EBlockType BlockType)
{
	if (UVsSetForBlock == BlockType)
		return;
	UVsSetForBlock = BlockType;

	const TArray<FVector> Normals;
	const TArray<FLinearColor> VertexColors;
	const TArray<FProcMeshTangent> Tangents;

	const auto& BlockInfo = (*BlockSettings->GetBlockInfoMap())[BlockType];

	UVs.Append(BlockInfo->SideUVs);
	UVs.Append(BlockInfo->SideUVs);
	UVs.Append(BlockInfo->SideUVs);
	UVs.Append(BlockInfo->SideUVs);
	UVs.Append(BlockInfo->TopUVs);
	UVs.Append(BlockInfo->BottomUVs);

	ProceduralMeshComponent->UpdateMeshSection_LinearColor(0, Vertices, Normals, UVs, VertexColors, Tangents);

	UVs.Empty();
}

void ABlockTerrainManipulator::UpdateSelectedBlock(bool bVisible)
{
	const EBlockType SelectedBlock = GetSelectedBlock();
	if (SelectedBlock == EBlockType::None)
	{
		Show(false);
	}
	else
	{
		UpdateUVs(SelectedBlock);
		Show(bVisible);
	}
}

void ABlockTerrainManipulator::Show(bool bNewVisibility) const
{
	if (ProceduralMeshComponent->IsMeshSectionVisible(0) == bNewVisibility)
		return;

	ProceduralMeshComponent->SetMeshSectionVisible(0, bNewVisibility);
}
