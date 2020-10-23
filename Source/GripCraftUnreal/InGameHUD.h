// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once 

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "InGameHUD.generated.h"

class UInGameHUDWidget;

UCLASS(Abstract)
class AInGameHUD final : public AHUD
{
	GENERATED_BODY()

public:
	void ToggleMainMenu();


private:
	UPROPERTY(EditDefaultsOnly)
	class UTexture2D* CrosshairTex;

	UPROPERTY(EditDefaultsOnly, Category="Widgets")
	TSubclassOf<UInGameHUDWidget> InGameHUDWidgetClass;

	UPROPERTY()
	UInGameHUDWidget* InGameHUDWidget;

	bool bMainMenuVisible = false;
	
	virtual void BeginPlay() override;
	virtual void DrawHUD() override;

	void DrawCrosshair() const;
};
