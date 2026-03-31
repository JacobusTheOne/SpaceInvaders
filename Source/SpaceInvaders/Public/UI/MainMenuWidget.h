// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuWidget.generated.h"

class UVolumeSettingsWidget;

UCLASS()
class SPACEINVADERS_API UMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// Bound to the Play button OnClicked event in the WBP Blueprint
	UFUNCTION(BlueprintCallable, Category = "Main Menu")
	void PlayGame();

	// Bound to the Quit button OnClicked event in the WBP Blueprint
	UFUNCTION(BlueprintCallable, Category = "Main Menu")
	void QuitGame();

	// Bound to the Settings button OnClicked event in the WBP Blueprint
	UFUNCTION(BlueprintCallable, Category = "Main Menu")
	void OpenSettings();

private:
	UPROPERTY(EditDefaultsOnly, Category = "Main Menu")
	FName GameLevelName = TEXT("Defaultmap");

	UPROPERTY(EditDefaultsOnly, Category = "Main Menu")
	TSubclassOf<UVolumeSettingsWidget> SettingsWidgetClass;
};
