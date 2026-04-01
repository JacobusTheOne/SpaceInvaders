// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "HUD/SpaceInvadersHUDWidget.h"
#include "SpaceInvaderGameModeBase.generated.h"

class UPauseMenuWidget;
class ASoundManager;

UCLASS()
class SPACEINVADERS_API ASpaceInvaderGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	void RespawnPlayer(AController* Controller);

	// Opens the pause menu if closed; closes it if open. Called by PlayerController on ESC.
	void TogglePause();

	UFUNCTION(BlueprintImplementableEvent, Category = "Game Mode")
	void GameOver();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD")
	USpaceInvadersHUDWidget* HUDWidget = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Pause")
	TSubclassOf<UPauseMenuWidget> PauseMenuWidgetClass;

	// Spawned automatically if no SoundManager is found in the level.
	// Assign BP_SoundManager here so the gameplay level never needs one placed manually.
	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	TSubclassOf<ASoundManager> SoundManagerClass;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Game Mode")
	float RespawnDelay = 2.f;

	FTimerHandle RespawnTimerHandle;

	UPROPERTY()
	UPauseMenuWidget* ActivePauseMenu = nullptr;
};
