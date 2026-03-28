// Fill out your copyright notice in the Description page of Project Settings.

#include "GameModes/MainMenuGameMode.h"
#include "UI/MainMenuWidget.h"
#include "Blueprint/UserWidget.h"

void AMainMenuGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (!MainMenuWidgetClass) return;

	MainMenuWidgetInstance = CreateWidget<UMainMenuWidget>(GetWorld(), MainMenuWidgetClass);
	if (!MainMenuWidgetInstance) return;

	MainMenuWidgetInstance->AddToViewport();

	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (PC)
	{
		PC->bShowMouseCursor = true;
		PC->SetInputMode(FInputModeUIOnly());
	}
}
