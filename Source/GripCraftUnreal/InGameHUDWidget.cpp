// Fill out your copyright notice in the Description page of Project Settings.


#include "InGameHUDWidget.h"

#include "InGameHUD.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"


void UInGameHUDWidget::NativeConstruct()
{
	ResumeButton->OnClicked.AddUniqueDynamic(this, &UInGameHUDWidget::OnResumeButtonClicked);
	QuitToMenuButton->OnClicked.AddUniqueDynamic(this, &UInGameHUDWidget::OnQuitToMenuButtonClicked);
}

void UInGameHUDWidget::OnResumeButtonClicked()
{
	// temp hack
	const APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	AInGameHUD* InGameHUD = Cast<AInGameHUD>(PlayerController->GetHUD());
	InGameHUD->ToggleMainMenu();
}

void UInGameHUDWidget::OnQuitToMenuButtonClicked()
{
	UGameplayStatics::OpenLevel(GetWorld(), TEXT("MainMenuMap"));
}
