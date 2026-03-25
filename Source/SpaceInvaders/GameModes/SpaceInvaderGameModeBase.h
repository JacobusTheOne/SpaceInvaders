// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "HUD/SpaceInvadersHUDWidget.h"
#include "SpaceInvaderGameModeBase.generated.h"

UCLASS()
class SPACEINVADERS_API ASpaceInvaderGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	void RespawnPlayer(AController* Controller);

	UFUNCTION(BlueprintImplementableEvent, Category = "Game Mode")
	void GameOver();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD")
	USpaceInvadersHUDWidget* HUDWidget = nullptr;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Game Mode")
	float RespawnDelay = 2.f;

	FTimerHandle RespawnTimerHandle;
};
