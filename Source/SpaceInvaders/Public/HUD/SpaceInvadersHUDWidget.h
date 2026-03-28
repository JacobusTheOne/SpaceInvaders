// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SpaceInvadersHUDWidget.generated.h"

class UTextBlock;
class UProgressBar;
class ASpaceInvaderGameState;
class AWaveManager;
class ABossEnemy;

UCLASS()
class SPACEINVADERS_API USpaceInvadersHUDWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	void UpdateDisplay();
	void UpdateBossDisplay(float DeltaTime);
	void BindToBoss(ABossEnemy* Boss);

	UFUNCTION()
	void OnWaveStarted(int32 WaveNumber);

	UFUNCTION()
	void OnBossHealthChanged(int32 CurrentHealth, int32 MaxHealth);

	// Names must match exactly the widget names in your WBP Blueprint
	UPROPERTY(meta = (BindWidget))
	UTextBlock* ScoreText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* LivesText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* WaveText;

	// Boss health UI — add a UProgressBar named BossHealthBar and
	// a UTextBlock named BossHealthText to your WBP Blueprint
	UPROPERTY(meta = (BindWidget))
	UProgressBar* BossHealthBar;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* BossHealthText;

	UPROPERTY()
	ASpaceInvaderGameState* GameState = nullptr;

	UPROPERTY()
	AWaveManager* WaveManager = nullptr;

	UPROPERTY()
	ABossEnemy* CachedBoss = nullptr;

	float DamageFlashElapsed = 0.f;

	static constexpr float DamageFlashDuration = 0.35f;
};
