// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "GripCraftUnrealGameInstance.generated.h"

UCLASS()
class GRIPCRAFTUNREAL_API UGripCraftUnrealGameInstance final : public UGameInstance
{
	GENERATED_BODY()

public:
	void LoadGameLevel() const;
	void LoadMainMenuLevel() const;
	void QuitGame() const;
};
