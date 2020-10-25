// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuWidget.h"
#include "Components/Button.h"
#include "GripCraftUnreal/GripCraftUnrealGameInstance.h"

void UMainMenuWidget::NativeConstruct()
{
	PlayButton->OnClicked.AddUniqueDynamic(this, &UMainMenuWidget::OnPlayButtonClicked);
	QuitButton->OnClicked.AddUniqueDynamic(this, &UMainMenuWidget::OnQuitButtonClicked);
}

void UMainMenuWidget::OnPlayButtonClicked()
{
	GetGameInstance<UGripCraftUnrealGameInstance>()->LoadGameLevel();
}

void UMainMenuWidget::OnQuitButtonClicked()
{
	GetGameInstance<UGripCraftUnrealGameInstance>()->QuitGame();
}
