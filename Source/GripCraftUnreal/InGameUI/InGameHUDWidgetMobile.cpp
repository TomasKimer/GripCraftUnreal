// Fill out your copyright notice in the Description page of Project Settings.


#include "InGameHUDWidgetMobile.h"
#include "InGameHUD.h"
#include "Components/Button.h"
#include "GripCraftUnreal/GripCraftUnrealCharacter.h"


void UInGameHUDWidgetMobile::NativeConstruct()
{
	MenuButton->OnClicked.AddUniqueDynamic(this, &UInGameHUDWidgetMobile::OnMenuButtonClicked);
	FireButton->OnClicked.AddUniqueDynamic(this, &UInGameHUDWidgetMobile::OnFireButtonClicked);
	JumpButton->OnClicked.AddUniqueDynamic(this, &UInGameHUDWidgetMobile::OnJumpButtonClicked);
}

void UInGameHUDWidgetMobile::OnMenuButtonClicked()
{
	Owner->ToggleMenu();
}

void UInGameHUDWidgetMobile::OnFireButtonClicked()
{
	AGripCraftUnrealCharacter* Character = Cast<AGripCraftUnrealCharacter>(Owner->GetOwningPawn());
	Character->OnFire();
}

void UInGameHUDWidgetMobile::OnJumpButtonClicked()
{
	AGripCraftUnrealCharacter* Character = Cast<AGripCraftUnrealCharacter>(Owner->GetOwningPawn());
	Character->Jump();
}
