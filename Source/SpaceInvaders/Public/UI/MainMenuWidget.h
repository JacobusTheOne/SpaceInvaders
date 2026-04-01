// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuWidget.generated.h"

class UVolumeSettingsWidget;
class UWidget;

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

protected:
	virtual void NativeConstruct() override;

private:
	void OnSettingsClosed();

	UPROPERTY(EditDefaultsOnly, Category = "Main Menu")
	FName GameLevelName = TEXT("Defaultmap");

	// Overlay containing the main menu buttons — name it MainMenuOverlay in WBP
	UPROPERTY(meta = (BindWidgetOptional))
	UWidget* MainMenuOverlay;

	// Embed WBP_VolumeSettings inside WBP_MainMenu in UMG and name it VolumeSettingsPanel
	UPROPERTY(meta = (BindWidgetOptional))
	UVolumeSettingsWidget* VolumeSettingsPanel;
};
