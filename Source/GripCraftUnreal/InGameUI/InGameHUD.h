// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once 

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "InGameHUD.generated.h"

class UInGameHUDWidget;
class UInGameHUDWidgetPC;
class UInGameHUDWidgetMobile;
class UInGameMenuWidget;
class UTexture2D;

UCLASS(Abstract)
class AInGameHUD final : public AHUD
{
	GENERATED_BODY()

public:
	void ToggleMainMenu();

private:
	UPROPERTY(EditDefaultsOnly)
	UTexture2D* CrosshairTex;

	UPROPERTY(EditDefaultsOnly, Category="Widgets")
	TSubclassOf<UInGameHUDWidgetPC> HUDWidgetClassPC;

	UPROPERTY(EditDefaultsOnly, Category="Widgets")
	TSubclassOf<UInGameHUDWidgetMobile> HUDWidgetClassMobile;

	UPROPERTY(EditDefaultsOnly, Category="Widgets")
	TSubclassOf<UInGameMenuWidget> MenuWidgetClass;

	UPROPERTY()
	UInGameHUDWidget* HUDWidget;

	UPROPERTY()
	UInGameMenuWidget* MenuWidget;

	bool bMainMenuVisible = false;

	virtual void BeginPlay() override;
	virtual void DrawHUD() override;

	void DrawCrosshair() const;
};
