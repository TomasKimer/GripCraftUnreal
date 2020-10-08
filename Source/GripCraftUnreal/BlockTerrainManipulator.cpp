// Fill out your copyright notice in the Description page of Project Settings.


#include "BlockTerrainManipulator.h"
#include "BlockTerrainChunk.h"
#include "DrawDebugHelpers.h"
#include "ProceduralMeshComponent.h"
#include "Kismet/KismetSystemLibrary.h"


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
	UpdateUVs(BlockVariants[0]);
}


void ABlockTerrainManipulator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


void ABlockTerrainManipulator::Show(bool bVisible) const
{
	ProceduralMeshComponent->SetMeshSectionVisible(0, bVisible);
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

	UpdateUVs(BlockVariants[SelectedBlockIndex]);
}


void ABlockTerrainManipulator::Update(FVector ViewOrigin, FVector ViewDirection)
{
	const FCollisionQueryParams params;
	FHitResult hitResult;
	const int blockSize = BlockSettings->BlockSize;
	const FVector endLocation = ViewOrigin + ViewDirection * MaxRaycastDistance * blockSize;

	if (GetWorld()->LineTraceSingleByChannel(hitResult, ViewOrigin, endLocation, ECC_GameTraceChannel2, params) == true)
	{
		FVector hitBlockPosition = hitResult.Location + hitResult.Normal * blockSize * 0.5f;

		DrawDebugLine(GetWorld(), hitResult.Location, hitBlockPosition, FColor(0, 255, 0));

		const AActor* hitActor = hitResult.Actor.Get(); // a weak pointer to the Actor that the trace hit

		if (hitActor->IsA(ABlockTerrainChunk::StaticClass()))
		{
			hitBlockPosition.X = blockSize * FMath::FloorToInt(hitBlockPosition.X / blockSize);
			hitBlockPosition.Y = blockSize * FMath::FloorToInt(hitBlockPosition.Y / blockSize);
			hitBlockPosition.Z = blockSize * FMath::FloorToInt(hitBlockPosition.Z / blockSize);

			SetActorLocation(hitBlockPosition);

			Show(true);
        }
		else
		{
			Show(false);
		}
	}
	else
	{
		Show(false);
	}
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

