// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InGameHUDWidget.h"
#include "InGameHUDWidgetMobile.generated.h"

class UButton;

UCLASS(Abstract)
class GRIPCRAFTUNREAL_API UInGameHUDWidgetMobile final : public UInGameHUDWidget
{
	GENERATED_BODY()

	UPROPERTY(meta = (BindWidget))
	UButton* MenuButton;

	UPROPERTY(meta = (BindWidget))
	UButton* FireButton;

	UPROPERTY(meta = (BindWidget))
	UButton* AlternateFireButton;

	UPROPERTY(meta = (BindWidget))
	UButton* ChangeBlockButton;

	UPROPERTY(meta = (BindWidget))
	UButton* JumpButton;

	UFUNCTION()
    void OnMenuButtonClicked();

	UFUNCTION()
	void OnFireButtonPressed();

	UFUNCTION()
    void OnAlternateFireButtonPressed();

	UFUNCTION()
    void OnChangeBlockButtonClicked();

	UFUNCTION()
	void OnJumpButtonPressed();

	virtual void NativeConstruct() override;
};
