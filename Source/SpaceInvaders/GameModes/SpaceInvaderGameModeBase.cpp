// Fill out your copyright notice in the Description page of Project Settings.

#include "GameModes/SpaceInvaderGameModeBase.h"

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
