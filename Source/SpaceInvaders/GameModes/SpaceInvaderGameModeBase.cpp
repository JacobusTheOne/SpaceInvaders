// Fill out your copyright notice in the Description page of Project Settings.

#include "GameModes/SpaceInvaderGameModeBase.h"
#include "UI/PauseMenuWidget.h"
#include "Kismet/GameplayStatics.h"

void ASpaceInvaderGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		PC->bShowMouseCursor = false;
		PC->SetInputMode(FInputModeGameOnly());
	}

	if (HUDWidget)
	{
		HUDWidget->AddToViewport();
	}
}

void ASpaceInvaderGameModeBase::TogglePause()
{
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC) return;

	if (IsValid(ActivePauseMenu))
	{
		ActivePauseMenu->CloseMenu();
		ActivePauseMenu = nullptr;
	}
	else
	{
		if (!PauseMenuWidgetClass) return;

		UGameplayStatics::SetGamePaused(this, true);
		ActivePauseMenu = CreateWidget<UPauseMenuWidget>(PC, PauseMenuWidgetClass);
		ActivePauseMenu->AddToViewport();
		PC->bShowMouseCursor = true;
		PC->SetInputMode(FInputModeGameAndUI());
	}
}

void ASpaceInvaderGameModeBase::RespawnPlayer(AController* Controller)
{
	if (!Controller) return;

	FTimerDelegate RespawnDelegate;
	RespawnDelegate.BindLambda([this, Controller]()
	{
		if (IsValid(Controller))
		{
			RestartPlayer(Controller);
		}
	});

	GetWorldTimerManager().SetTimer(RespawnTimerHandle, RespawnDelegate, RespawnDelay, false);
}
