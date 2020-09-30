// Fill out your copyright notice in the Description page of Project Settings.


#include "BlockTerrainChunk.h"
#include "ProceduralMeshComponent.h"
#include "KismetProceduralMeshLibrary.h"

// Sets default values
ABlockTerrainChunk::ABlockTerrainChunk()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// any of following returns null, why?
//	ProceduralMeshComp = Cast<UProceduralMeshComponent>(GetComponentByClass(UProceduralMeshComponent::StaticClass()));
//	ProceduralMeshComp = FindComponentByClass<UProceduralMeshComponent>()

	// used this instead
	ProceduralMeshComp = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProcMeshComp"));
}

// Called when the game starts or when spawned
void ABlockTerrainChunk::BeginPlay()
{
	Super::BeginPlay();

	CreateMesh();

	if (ProceduralMeshComp != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("ProceduralMeshComp present"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ProceduralMeshComp NOT present"));
	}
}

// Called every frame
void ABlockTerrainChunk::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABlockTerrainChunk::CreateMesh()
{
	const int   FACE_COUNT        = 6;
	const int   INDICES_PER_FACE  = 6;
	const int   VERTICES_PER_FACE = 4;
	const int   VERTEX_COUNT      = FACE_COUNT * VERTICES_PER_FACE;
	const int   INDEX_COUNT       = FACE_COUNT * INDICES_PER_FACE;	
	const int   CUBE_SIZE_IN_CM   = 100;
	const float TILE_SIZE         = 16.0f;

	TArray<FVector> vertices = {
		// left
		FVector(0, 1, 0),
		FVector(0, 1, 1),
		FVector(0, 0, 1),
		FVector(0, 0, 0),

		// right
		FVector(1, 0, 0),
		FVector(1, 0, 1),
		FVector(1, 1, 1),
		FVector(1, 1, 0),

		// front
		FVector(0, 0, 0),
		FVector(0, 0, 1),
		FVector(1, 0, 1),
		FVector(1, 0, 0),

		// back
		FVector(1, 1, 0),
		FVector(1, 1, 1),
		FVector(0, 1, 1),
		FVector(0, 1, 0),

		// top
		FVector(0, 0, 1),
		FVector(0, 1, 1),
		FVector(1, 1, 1),
		FVector(1, 0, 1),

		// bottom
		FVector(0, 0, 0),
		FVector(1, 0, 0),
		FVector(1, 1, 0),
		FVector(0, 1, 0),
	};

	for (FVector& v : vertices)
	{
		v *= CUBE_SIZE_IN_CM;
	}

	TArray<int32> triangles;
	
	for (int i = 0; i < FACE_COUNT; ++i)
	{
		int32 idx = i * VERTICES_PER_FACE;

		triangles.Add(idx);
		triangles.Add(idx + 1);
		triangles.Add(idx + 2);

		triangles.Add(idx);
		triangles.Add(idx + 2);
		triangles.Add(idx + 3);
	}
	
	TArray<FVector2D> uv0 {
		//side
		FVector2D(0.0f / TILE_SIZE, 0.0f / TILE_SIZE),
		FVector2D(0.0f / TILE_SIZE, 1.0f / TILE_SIZE),
		FVector2D(1.0f / TILE_SIZE, 1.0f / TILE_SIZE),
		FVector2D(1.0f / TILE_SIZE, 0.0f / TILE_SIZE),

		//side
		FVector2D(0.0f / TILE_SIZE, 0.0f / TILE_SIZE),
		FVector2D(0.0f / TILE_SIZE, 1.0f / TILE_SIZE),
		FVector2D(1.0f / TILE_SIZE, 1.0f / TILE_SIZE),
		FVector2D(1.0f / TILE_SIZE, 0.0f / TILE_SIZE),

		//side
		FVector2D(0.0f / TILE_SIZE, 0.0f / TILE_SIZE),
		FVector2D(0.0f / TILE_SIZE, 1.0f / TILE_SIZE),
		FVector2D(1.0f / TILE_SIZE, 1.0f / TILE_SIZE),
		FVector2D(1.0f / TILE_SIZE, 0.0f / TILE_SIZE),

		//side
		FVector2D(0.0f / TILE_SIZE, 0.0f / TILE_SIZE),
		FVector2D(0.0f / TILE_SIZE, 1.0f / TILE_SIZE),
		FVector2D(1.0f / TILE_SIZE, 1.0f / TILE_SIZE),
		FVector2D(1.0f / TILE_SIZE, 0.0f / TILE_SIZE),

		//top
		FVector2D(0.0f / TILE_SIZE, 0.0f / TILE_SIZE),
		FVector2D(0.0f / TILE_SIZE, 1.0f / TILE_SIZE),
		FVector2D(1.0f / TILE_SIZE, 1.0f / TILE_SIZE),
		FVector2D(1.0f / TILE_SIZE, 0.0f / TILE_SIZE),

		//bottom
		FVector2D(0.0f / TILE_SIZE, 0.0f / TILE_SIZE),
		FVector2D(0.0f / TILE_SIZE, 1.0f / TILE_SIZE),
		FVector2D(1.0f / TILE_SIZE, 1.0f / TILE_SIZE),
		FVector2D(1.0f / TILE_SIZE, 0.0f / TILE_SIZE),
	};
	
	TArray<FVector> normals;
	TArray<FProcMeshTangent> tangents;
	TArray<FLinearColor> vertexColors;

	UKismetProceduralMeshLibrary::CalculateTangentsForMesh(vertices, triangles, uv0, normals, tangents);

	ProceduralMeshComp->CreateMeshSection_LinearColor(0, vertices, triangles, normals, uv0, vertexColors, tangents, true);
	ProceduralMeshComp->SetMaterial(0, Material);
}

