// Fill out your copyright notice in the Description page of Project Settings.

#include "GameState/SpaceInvaderGameState.h"
#include "Kismet/GameplayStatics.h"

void ASpaceInvaderGameState::TriggerGameOver()
{
	UE_LOG(LogTemp, Log, TEXT("GameState: GameOver triggered — reloading level"));
	UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetMapName()));
}

void ASpaceInvaderGameState::AddScore(int32 Amount)
{
	Score += Amount;
}

void ASpaceInvaderGameState::SetScore(int32 NewScore)
{
	Score = NewScore;
}

void ASpaceInvaderGameState::SetLives(int32 NewLives)
{
	Lives = NewLives;
}

void ASpaceInvaderGameState::LoseLife()
{
	Lives = FMath::Max(0, Lives - 1);
}

void ASpaceInvaderGameState::SetWaveNumber(int32 NewWaveNumber)
{
	WaveNumber = NewWaveNumber;
}

void ASpaceInvaderGameState::AdvanceWave()
{
	WaveNumber++;
}
