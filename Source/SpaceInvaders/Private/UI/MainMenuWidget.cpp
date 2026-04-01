// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/MainMenuWidget.h"
#include "UI/VolumeSettingsWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

void UMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (VolumeSettingsPanel)
	{
		VolumeSettingsPanel->OnClosed.AddUObject(this, &UMainMenuWidget::OnSettingsClosed);
	}
}

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
	if (MainMenuOverlay)
	{
		MainMenuOverlay->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (VolumeSettingsPanel)
	{
		VolumeSettingsPanel->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
}

void UMainMenuWidget::OnSettingsClosed()
{
	if (MainMenuOverlay)
	{
		MainMenuOverlay->SetVisibility(ESlateVisibility::Visible);
	}
}
