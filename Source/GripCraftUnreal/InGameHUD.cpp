// Copyright Epic Games, Inc. All Rights Reserved.

#include "InGameHUD.h"
#include "Engine/Canvas.h"
#include "Engine/Texture2D.h"
#include "CanvasItem.h"
#include "InGameHUDWidget.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Kismet/GameplayStatics.h"


void AInGameHUD::ToggleMainMenu()
{
	if (InGameHUDWidget == nullptr)
	{
		InGameHUDWidget = CreateWidget<UInGameHUDWidget>(GetWorld(), InGameHUDWidgetClass);
	}

	bMainMenuVisible = !bMainMenuVisible;

	if (bMainMenuVisible == true)
	{
		InGameHUDWidget->AddToViewport();

		UWidgetBlueprintLibrary::SetInputMode_UIOnlyEx(GetOwningPlayerController(), InGameHUDWidget, EMouseLockMode::DoNotLock);
		UGameplayStatics::SetGamePaused(GetWorld(), true);
	}
	else
	{
		UGameplayStatics::SetGamePaused(GetWorld(), false);
		GetOwningPlayerController()->SetInputMode(FInputModeGameOnly());

		InGameHUDWidget->RemoveFromViewport();
	}
}

void AInGameHUD::BeginPlay()
{
	Super::BeginPlay();
}

void AInGameHUD::DrawHUD()
{
	Super::DrawHUD();

	DrawCrosshair();
}

void AInGameHUD::DrawCrosshair() const
{
	// find center of the Canvas
	const FVector2D Center(Canvas->ClipX * 0.5f,
	                       Canvas->ClipY * 0.5f);

	// offset by half the texture's dimensions so that the center of the texture aligns with the center of the Canvas
	const FVector2D CrosshairDrawPosition((Center.X - CrosshairTex->GetSizeX() * 0.5f),
                                          (Center.Y - CrosshairTex->GetSizeY() * 0.5f));

	// draw the crosshair
	FCanvasTileItem TileItem(CrosshairDrawPosition, CrosshairTex->Resource, FLinearColor::White);
	TileItem.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem(TileItem);
}
