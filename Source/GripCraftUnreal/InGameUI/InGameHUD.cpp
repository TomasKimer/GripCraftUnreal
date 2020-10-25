// Copyright Epic Games, Inc. All Rights Reserved.

#include "InGameHUD.h"
#include "Engine/Canvas.h"
#include "Engine/Texture2D.h"
#include "CanvasItem.h"
#include "InGameHUDWidget.h"
#include "InGameHUDWidgetPC.h"
#include "InGameHUDWidgetMobile.h"
#include "InGameMenuWidget.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "GameFramework/InputSettings.h"
#include "Kismet/GameplayStatics.h"


void AInGameHUD::ToggleMenu()
{
	if (MenuWidget == nullptr)
	{
		MenuWidget = CreateWidget<UInGameMenuWidget>(GetWorld(), MenuWidgetClass);
		MenuWidget->SetOwner(this);
	}

	bMenuVisible = !bMenuVisible;

	if (bMenuVisible == true)
	{
		HUDWidget->RemoveFromViewport();
		MenuWidget->AddToViewport();

		UWidgetBlueprintLibrary::SetInputMode_UIOnlyEx(GetOwningPlayerController(), MenuWidget, EMouseLockMode::DoNotLock);
		UGameplayStatics::SetGamePaused(GetWorld(), true);
	}
	else
	{
		UGameplayStatics::SetGamePaused(GetWorld(), false);
		GetOwningPlayerController()->SetInputMode(FInputModeGameOnly());

		MenuWidget->RemoveFromViewport();
		HUDWidget->AddToViewport();
	}
}

void AInGameHUD::BeginPlay()
{
	Super::BeginPlay();

	// this code is taken from GripCraftUnrealCharacter.cpp template
	// but SVirtualJoystick::ShouldDisplayTouchInterface() should be more consistent with showing virtual joysticks (e.g. in the emulator)
	// but I'm getting "unresolved external symbol" when using it, so using the code from the template for now 
	if (FPlatformMisc::SupportsTouchInput() || GetDefault<UInputSettings>()->bUseMouseForTouch)
	{
		HUDWidget = CreateWidget<UInGameHUDWidgetMobile>(GetWorld(), HUDWidgetClassMobile);
	}
	else
	{
		HUDWidget = CreateWidget<UInGameHUDWidgetPC>(GetWorld(), HUDWidgetClassPC);
	}

	HUDWidget->SetOwner(this);
	HUDWidget->AddToViewport();
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
