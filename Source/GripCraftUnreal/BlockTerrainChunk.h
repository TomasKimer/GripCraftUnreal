// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BlockTerrainChunk.generated.h"

UCLASS()
class GRIPCRAFTUNREAL_API ABlockTerrainChunk : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABlockTerrainChunk();

private:
	UPROPERTY(VisibleDefaultsOnly)
	class UProceduralMeshComponent* ProceduralMeshComp;

	UPROPERTY(EditAnywhere)
	class UMaterialInterface* Material;
	
	void CreateMesh();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
