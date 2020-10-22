// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MainMenuHUD.generated.h"

class UMainMenuWidget;

UCLASS(Abstract)
class GRIPCRAFTUNREAL_API AMainMenuHUD final : public AHUD
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category="Widgets")
	TSubclassOf<UMainMenuWidget> MainMenuWidgetClass;

	UPROPERTY()
	UMainMenuWidget* MainMenuWidget;

	virtual void BeginPlay() override;
};
