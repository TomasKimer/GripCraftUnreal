// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once 

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "GripCraftUnrealHUD.generated.h"

UCLASS()
class AGripCraftUnrealHUD : public AHUD
{
	GENERATED_BODY()

public:
	AGripCraftUnrealHUD();

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;

private:
	/** Crosshair asset pointer */
	class UTexture2D* CrosshairTex;
};
