// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SpaceInvadersHUDWidget.generated.h"

class UTextBlock;
class ASpaceInvaderGameState;
class AWaveManager;

UCLASS()
class SPACEINVADERS_API USpaceInvadersHUDWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	void UpdateDisplay();

	UFUNCTION()
	void OnWaveStarted(int32 WaveNumber);

	// Names must match exactly the widget names in your WBP Blueprint
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ScoreText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* LivesText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* WaveText;

	UPROPERTY()
	ASpaceInvaderGameState* GameState = nullptr;

	UPROPERTY()
	AWaveManager* WaveManager = nullptr;
};
