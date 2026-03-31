// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/MainMenuWidget.h"
#include "UI/VolumeSettingsWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

void UMainMenuWidget::PlayGame()
{
	UGameplayStatics::OpenLevel(this, GameLevelName);
}

void UMainMenuWidget::QuitGame()
{
	UKismetSystemLibrary::QuitGame(this, nullptr, EQuitPreference::Quit, false);
}

void UMainMenuWidget::OpenSettings()
{
	if (!SettingsWidgetClass) return;
	CreateWidget<UVolumeSettingsWidget>(GetOwningPlayer(), SettingsWidgetClass)->AddToViewport();
}
