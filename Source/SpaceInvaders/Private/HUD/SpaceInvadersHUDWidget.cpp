// Fill out your copyright notice in the Description page of Project Settings.

#include "HUD/SpaceInvadersHUDWidget.h"
#include "GameState/SpaceInvaderGameState.h"
#include "Managers/WaveManager.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"

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
