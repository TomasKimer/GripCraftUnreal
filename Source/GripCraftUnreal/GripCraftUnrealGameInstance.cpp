// Fill out your copyright notice in the Description page of Project Settings.


#include "GripCraftUnrealGameInstance.h"
#include "Kismet/GameplayStatics.h"


void UGripCraftUnrealGameInstance::LoadGameLevel(bool bFromSave)
{
	bLoadGameLevelFromSave = bFromSave;

	UGameplayStatics::OpenLevel(GetWorld(), TEXT("/Game/Maps/GameMap"));
}

void UGripCraftUnrealGameInstance::LoadMainMenuLevel() const
{
	UGameplayStatics::OpenLevel(GetWorld(), TEXT("/Game/Maps/MainMenuMap"));
}

void UGripCraftUnrealGameInstance::QuitGame() const
{
	UKismetSystemLibrary::QuitGame(GetWorld(), nullptr, EQuitPreference::Quit, false);
}
