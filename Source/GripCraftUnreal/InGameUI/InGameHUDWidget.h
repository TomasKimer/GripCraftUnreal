// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InGameHUDWidget.generated.h"

class AInGameHUD;

UCLASS(Abstract)
class GRIPCRAFTUNREAL_API UInGameHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	FORCEINLINE void SetOwner(AInGameHUD* InOwner) { Owner = InOwner; }

protected:
	UPROPERTY()
	AInGameHUD* Owner;
};
