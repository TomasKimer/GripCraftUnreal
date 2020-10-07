// Copyright Epic Games, Inc. All Rights Reserved.

#include "GripCraftUnrealProjectile.h"

#include "BlockTerrainChunk.h"
#include "BlockTerrainManager.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"

AGripCraftUnrealProjectile::AGripCraftUnrealProjectile() 
{
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &AGripCraftUnrealProjectile::OnHit);		// set up a notification for when this component hits something blocking

	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Set as root component
	RootComponent = CollisionComp;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;

	// Die after 10 seconds by default
	InitialLifeSpan = 10.0f;
}

void AGripCraftUnrealProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor == nullptr)
		return;
	if (OtherActor == this)
		return;

	if ((OtherComp != nullptr) && OtherComp->IsSimulatingPhysics())
	{
		OtherComp->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());
	}
	else if (OtherActor->IsA(ABlockTerrainChunk::StaticClass()))
	{
		ABlockTerrainManager* blockTerrainManager = Cast<ABlockTerrainManager>(OtherActor->GetAttachParentActor());
		if (blockTerrainManager != nullptr)
		{
			blockTerrainManager->DamageBlock(Hit.ImpactPoint, Hit.Normal, Damage);
		//	blockTerrainManager->AddBlock(Hit.ImpactPoint, Hit.Normal, EBlockType::Stone);

			if (bDestroyOnTerrainHit)
			{
				Destroy();
			}
		}
	}
}