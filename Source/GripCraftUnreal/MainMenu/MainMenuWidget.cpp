// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

void UMainMenuWidget::NativeConstruct()
{
	PlayButton->OnClicked.AddUniqueDynamic(this, &UMainMenuWidget::OnPlayButtonClicked);
	QuitButton->OnClicked.AddUniqueDynamic(this, &UMainMenuWidget::OnQuitButtonClicked);
}

void UMainMenuWidget::OnPlayButtonClicked()
{
	UGameplayStatics::OpenLevel(GetWorld(), TEXT("GameMap"));
}

void UMainMenuWidget::OnQuitButtonClicked()
{
	UKismetSystemLibrary::QuitGame(GetWorld(), nullptr, EQuitPreference::Quit, false);
}
