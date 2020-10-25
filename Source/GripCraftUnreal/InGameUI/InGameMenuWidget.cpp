// Fill out your copyright notice in the Description page of Project Settings.


#include "InGameMenuWidget.h"
#include "InGameHUD.h"
#include "Components/Button.h"
#include "GripCraftUnreal/GripCraftUnrealGameInstance.h"
#include "Kismet/GameplayStatics.h"


void UInGameMenuWidget::NativeConstruct()
{
	ResumeButton->OnClicked.AddUniqueDynamic(this, &UInGameMenuWidget::OnResumeButtonClicked);
	QuitToMenuButton->OnClicked.AddUniqueDynamic(this, &UInGameMenuWidget::OnQuitToMenuButtonClicked);
}

void UInGameMenuWidget::OnResumeButtonClicked()
{
	// temp hack
	const APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	AInGameHUD* InGameHUD = Cast<AInGameHUD>(PlayerController->GetHUD());
	InGameHUD->ToggleMainMenu();
}

void UInGameMenuWidget::OnQuitToMenuButtonClicked()
{
	GetGameInstance<UGripCraftUnrealGameInstance>()->LoadMainMenuLevel();
}