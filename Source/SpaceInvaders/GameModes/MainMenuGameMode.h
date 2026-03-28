// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MainMenuGameMode.generated.h"

class UMainMenuWidget;

UCLASS()
class SPACEINVADERS_API AMainMenuGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

private:
	// Assign WBP_MainMenu (derived from UMainMenuWidget) here in the Blueprint CDO
	UPROPERTY(EditDefaultsOnly, Category = "Main Menu")
	TSubclassOf<UMainMenuWidget> MainMenuWidgetClass;

	UPROPERTY()
	UMainMenuWidget* MainMenuWidgetInstance = nullptr;
};
