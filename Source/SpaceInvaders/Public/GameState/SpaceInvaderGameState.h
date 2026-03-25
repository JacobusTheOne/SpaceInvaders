// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "SpaceInvaderGameState.generated.h"

UCLASS()
class SPACEINVADERS_API ASpaceInvaderGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	// Score
	UFUNCTION(BlueprintPure, Category = "Game State")
	int32 GetScore() const { return Score; }

	UFUNCTION(BlueprintCallable, Category = "Game State")
	void AddScore(int32 Amount);

	UFUNCTION(BlueprintCallable, Category = "Game State")
	void SetScore(int32 NewScore);

	// Lives
	UFUNCTION(BlueprintPure, Category = "Game State")
	int32 GetLives() const { return Lives; }

	UFUNCTION(BlueprintCallable, Category = "Game State")
	void SetLives(int32 NewLives);

	UFUNCTION(BlueprintCallable, Category = "Game State")
	void LoseLife();

	// Wave
	UFUNCTION(BlueprintPure, Category = "Game State")
	int32 GetWaveNumber() const { return WaveNumber; }

	UFUNCTION(BlueprintCallable, Category = "Game State")
	void SetWaveNumber(int32 NewWaveNumber);

	UFUNCTION(BlueprintCallable, Category = "Game State")
	void AdvanceWave();

private:
	UPROPERTY(VisibleAnywhere, Category = "Game State")
	int32 Score = 0;

	UPROPERTY(EditDefaultsOnly, Category = "Game State")
	int32 Lives = 3;

	UPROPERTY(VisibleAnywhere, Category = "Game State")
	int32 WaveNumber = 1;
};
