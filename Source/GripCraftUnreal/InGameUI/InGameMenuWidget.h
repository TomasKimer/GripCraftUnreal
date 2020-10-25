// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InGameMenuWidget.generated.h"

class UButton;

UCLASS(Abstract)
class GRIPCRAFTUNREAL_API UInGameMenuWidget final : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(meta = (BindWidget))
	UButton* ResumeButton;

	UPROPERTY(meta = (BindWidget))
	UButton* QuitToMenuButton;

	UFUNCTION()
    void OnResumeButtonClicked();

	UFUNCTION()
    void OnQuitToMenuButtonClicked();

	virtual void NativeConstruct() override;
};
