// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuWidget.generated.h"

class UButton;

UCLASS(Abstract)
class GRIPCRAFTUNREAL_API UMainMenuWidget final : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(meta = (BindWidget))
	UButton* PlayButton;

	UPROPERTY(meta = (BindWidget))
	UButton* LoadButton;

	UPROPERTY(meta = (BindWidget))
	UButton* QuitButton;

	UFUNCTION()
	void OnPlayButtonClicked();

	UFUNCTION()
    void OnLoadButtonClicked();

	UFUNCTION()
	void OnQuitButtonClicked();

	virtual void NativeConstruct() override;
};
