// Fill out your copyright notice in the Description page of Project Settings.


#include "InGameHUDWidgetMobile.h"
#include "InGameHUD.h"
#include "Components/Button.h"
#include "GripCraftUnreal/GripCraftUnrealCharacter.h"


void UInGameHUDWidgetMobile::NativeConstruct()
{
	MenuButton->OnClicked.AddUniqueDynamic(this, &UInGameHUDWidgetMobile::OnMenuButtonClicked);
	FireButton->OnPressed.AddUniqueDynamic(this, &UInGameHUDWidgetMobile::OnFireButtonPressed);
	JumpButton->OnPressed.AddUniqueDynamic(this, &UInGameHUDWidgetMobile::OnJumpButtonPressed);
}

void UInGameHUDWidgetMobile::OnMenuButtonClicked()
{
	Owner->ToggleMenu();
}

void UInGameHUDWidgetMobile::OnFireButtonPressed()
{
	AGripCraftUnrealCharacter* Character = Cast<AGripCraftUnrealCharacter>(Owner->GetOwningPawn());
	Character->OnFire();
}

void UInGameHUDWidgetMobile::OnJumpButtonPressed()
{
	AGripCraftUnrealCharacter* Character = Cast<AGripCraftUnrealCharacter>(Owner->GetOwningPawn());
	Character->Jump();
}
