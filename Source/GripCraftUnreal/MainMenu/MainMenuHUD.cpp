// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuHUD.h"
#include "MainMenuWidget.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Kismet/GameplayStatics.h"

void AMainMenuHUD::BeginPlay()
{
	Super::BeginPlay();

	if (MainMenuWidgetClass != nullptr)
	{
		MainMenuWidget = CreateWidget<UMainMenuWidget>(GetWorld(), MainMenuWidgetClass);

		if (MainMenuWidget != nullptr)
		{
			MainMenuWidget->AddToViewport();

			UWidgetBlueprintLibrary::SetInputMode_UIOnlyEx(UGameplayStatics::GetPlayerController(GetWorld(), 0), MainMenuWidget, EMouseLockMode::DoNotLock);
		}
	}
}
