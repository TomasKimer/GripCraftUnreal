// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuWidget.h"
#include "Components/Button.h"
#include "GripCraftUnreal/GripCraftUnrealGameInstance.h"
#include "GripCraftUnreal/GripCraftUnrealSaveGame.h"

void UMainMenuWidget::NativeConstruct()
{
	PlayButton->OnClicked.AddUniqueDynamic(this, &UMainMenuWidget::OnPlayButtonClicked);
	LoadButton->OnClicked.AddUniqueDynamic(this, &UMainMenuWidget::OnLoadButtonClicked);
	QuitButton->OnClicked.AddUniqueDynamic(this, &UMainMenuWidget::OnQuitButtonClicked);

	LoadButton->SetIsEnabled(FGripCraftUnrealSaveGame::DoesSaveGameExist());
}

void UMainMenuWidget::OnPlayButtonClicked()
{
	GetGameInstance<UGripCraftUnrealGameInstance>()->LoadGameLevel(false);
}

void UMainMenuWidget::OnLoadButtonClicked()
{
	GetGameInstance<UGripCraftUnrealGameInstance>()->LoadGameLevel(true);
}

void UMainMenuWidget::OnQuitButtonClicked()
{
	GetGameInstance<UGripCraftUnrealGameInstance>()->QuitGame();
}
