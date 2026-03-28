// Fill out your copyright notice in the Description page of Project Settings.

#include "HUD/SpaceInvadersHUDWidget.h"
#include "GameState/SpaceInvaderGameState.h"
#include "Managers/WaveManager.h"
#include "Ships/BossEnemy.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "Kismet/GameplayStatics.h"

static constexpr FLinearColor BarColorNormal = FLinearColor(0.75f, 0.08f, 0.08f);
static constexpr FLinearColor BarColorFlash  = FLinearColor(1.f,   0.85f, 0.f);

void USpaceInvadersHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	GameState = GetWorld()->GetGameState<ASpaceInvaderGameState>();

	WaveManager = Cast<AWaveManager>(
		UGameplayStatics::GetActorOfClass(GetWorld(), AWaveManager::StaticClass()));

	if (WaveManager)
	{
		WaveManager->OnWaveStarted.AddDynamic(this, &USpaceInvadersHUDWidget::OnWaveStarted);
	}

	UpdateDisplay();
}

void USpaceInvadersHUDWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	UpdateDisplay();
	UpdateBossDisplay(InDeltaTime);
}

void USpaceInvadersHUDWidget::OnWaveStarted(int32 WaveNumber)
{
	UpdateDisplay();
}

void USpaceInvadersHUDWidget::UpdateDisplay()
{
	if (!GameState) return;

	if (ScoreText)
	{
		ScoreText->SetText(FText::FromString(FString::Printf(TEXT("SCORE: %d"), GameState->GetScore())));
	}

	if (LivesText)
	{
		LivesText->SetText(FText::FromString(FString::Printf(TEXT("LIVES: %d"), GameState->GetLives())));
	}

	if (WaveText)
	{
		WaveText->SetText(FText::FromString(FString::Printf(TEXT("WAVE: %d"), GameState->GetWaveNumber())));
	}
}

void USpaceInvadersHUDWidget::UpdateBossDisplay(float DeltaTime)
{
	// Try to find and bind the boss if it has spawned since last check
	if (!IsValid(CachedBoss))
	{
		ABossEnemy* Found = Cast<ABossEnemy>(
			UGameplayStatics::GetActorOfClass(GetWorld(), ABossEnemy::StaticClass()));
		if (Found)
		{
			BindToBoss(Found);
		}
	}

	// Hide boss UI when no boss is active
	const bool bBossAlive = IsValid(CachedBoss);
	if (BossHealthBar)  BossHealthBar->SetVisibility(bBossAlive ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	if (BossHealthText) BossHealthText->SetVisibility(bBossAlive ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);

	// Advance the damage flash timer
	if (DamageFlashElapsed > 0.f)
	{
		DamageFlashElapsed -= DeltaTime;
		if (BossHealthBar)
			BossHealthBar->SetFillColorAndOpacity(BarColorFlash);
	}
	else
	{
		if (BossHealthBar)
			BossHealthBar->SetFillColorAndOpacity(BarColorNormal);
	}
}

void USpaceInvadersHUDWidget::BindToBoss(ABossEnemy* Boss)
{
	CachedBoss = Boss;
	DamageFlashElapsed = 0.f;

	// Initialise bar and text to full health immediately
	OnBossHealthChanged(CachedBoss->GetHealth(), CachedBoss->GetMaxHealth());

	CachedBoss->OnHealthChanged.AddDynamic(this, &USpaceInvadersHUDWidget::OnBossHealthChanged);
}

void USpaceInvadersHUDWidget::OnBossHealthChanged(int32 CurrentHealth, int32 MaxHealth)
{
	if (BossHealthBar)
	{
		BossHealthBar->SetPercent(static_cast<float>(CurrentHealth) / static_cast<float>(MaxHealth));
	}

	if (BossHealthText)
	{
		BossHealthText->SetText(FText::FromString(
			FString::Printf(TEXT("BOSS  %d / %d"), CurrentHealth, MaxHealth)));
	}

	DamageFlashElapsed = DamageFlashDuration;
}
