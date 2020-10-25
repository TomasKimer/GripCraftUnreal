// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InGameHUD.h"
#include "Blueprint/UserWidget.h"
#include "InGameMenuWidget.generated.h"

class AInGameHUD;
class UButton;

UCLASS(Abstract)
class GRIPCRAFTUNREAL_API UInGameMenuWidget final : public UUserWidget
{
	GENERATED_BODY()

public:
	FORCEINLINE void SetOwner(AInGameHUD* InOwner) { Owner = InOwner; }

private:
	UPROPERTY(meta = (BindWidget))
	UButton* ResumeButton;

	UPROPERTY(meta = (BindWidget))
	UButton* QuitToMenuButton;

	UPROPERTY()
	AInGameHUD* Owner;

	UFUNCTION()
    void OnResumeButtonClicked();

	UFUNCTION()
    void OnQuitToMenuButtonClicked();

	virtual void NativeConstruct() override;
};
