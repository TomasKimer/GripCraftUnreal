// Fill out your copyright notice in the Description page of Project Settings.


#include "InGameHUDWidgetMobile.h"
#include "InGameHUD.h"
#include "Components/Button.h"
#include "GripCraftUnreal/GripCraftUnrealCharacter.h"


void UInGameHUDWidgetMobile::NativeConstruct()
{
	MenuButton->OnClicked.AddUniqueDynamic(this, &UInGameHUDWidgetMobile::OnMenuButtonClicked);
	FireButton->OnPressed.AddUniqueDynamic(this, &UInGameHUDWidgetMobile::OnFireButtonPressed);
	AlternateFireButton->OnPressed.AddUniqueDynamic(this, &UInGameHUDWidgetMobile::OnAlternateFireButtonPressed);
	ChangeBlockButton->OnClicked.AddUniqueDynamic(this, &UInGameHUDWidgetMobile::OnChangeBlockButtonClicked);
	JumpButton->OnPressed.AddUniqueDynamic(this, &UInGameHUDWidgetMobile::OnJumpButtonPressed);
}

void UInGameHUDWidgetMobile::OnMenuButtonClicked()
{
	Owner->ToggleMenu();
}

void UInGameHUDWidgetMobile::OnFireButtonPressed()
{
	// FIXME GetOwningPlayer() == nullptr
	Cast<AGripCraftUnrealCharacter>(Owner->GetOwningPawn())->OnFire();
}

void UInGameHUDWidgetMobile::OnAlternateFireButtonPressed()
{
	Cast<AGripCraftUnrealCharacter>(Owner->GetOwningPawn())->OnAlternateFire();
}

void UInGameHUDWidgetMobile::OnChangeBlockButtonClicked()
{
	Cast<AGripCraftUnrealCharacter>(Owner->GetOwningPawn())->OnNextWeapon();
}

void UInGameHUDWidgetMobile::OnJumpButtonPressed()
{
	Cast<AGripCraftUnrealCharacter>(Owner->GetOwningPawn())->Jump();
}
