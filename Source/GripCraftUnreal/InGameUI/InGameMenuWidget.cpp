// Fill out your copyright notice in the Description page of Project Settings.


#include "InGameMenuWidget.h"
#include "InGameHUD.h"
#include "Components/Button.h"
#include "GripCraftUnreal/GripCraftUnrealGameInstance.h"


void UInGameMenuWidget::NativeConstruct()
{
	ResumeButton->OnClicked.AddUniqueDynamic(this, &UInGameMenuWidget::OnResumeButtonClicked);
	QuitToMenuButton->OnClicked.AddUniqueDynamic(this, &UInGameMenuWidget::OnQuitToMenuButtonClicked);
}

void UInGameMenuWidget::OnResumeButtonClicked()
{
	Owner->ToggleMenu();
}

void UInGameMenuWidget::OnQuitToMenuButtonClicked()
{
	GetGameInstance<UGripCraftUnrealGameInstance>()->LoadMainMenuLevel();
}