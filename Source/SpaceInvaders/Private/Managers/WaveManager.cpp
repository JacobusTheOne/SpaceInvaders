#include "Managers/WaveManager.h"
#include "Formation/EnemyFormation.h"
#include "GameState/SpaceInvaderGameState.h"
#include "UI/WaveClearedUI.h"
#include "Kismet/GameplayStatics.h"

AWaveManager::AWaveManager()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AWaveManager::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Log, TEXT("WaveManager: BeginPlay — %d wave(s) configured, FormationClass: %s"),
		Waves.Num(), FormationClass ? *FormationClass->GetName() : TEXT("NULL"));

	// If a formation was placed in the level instead of spawned, bind to it directly
	if (!IsValid(ActiveFormation))
	{
		ActiveFormation = Cast<AEnemyFormation>(
			UGameplayStatics::GetActorOfClass(GetWorld(), AEnemyFormation::StaticClass()));
	}

	if (IsValid(ActiveFormation))
	{
		UE_LOG(LogTemp, Log, TEXT("WaveManager: found existing formation '%s' — binding delegates"), *ActiveFormation->GetName());
		ActiveFormation->SetGameOverThreshold(GameOverXThreshold);
		ActiveFormation->OnFormationCleared.AddDynamic(this, &AWaveManager::OnFormationCleared);
		ActiveFormation->OnGameOver.AddDynamic(this, &AWaveManager::OnGameOverTriggered);
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("WaveManager: no existing formation found — calling StartWave(0)"));
		StartWave(0);
	}
}

void AWaveManager::StartWave(int32 WaveNumber)
{
	UE_LOG(LogTemp, Log, TEXT("WaveManager: StartWave(%d) called — Waves.Num=%d, FormationClass=%s"),
		WaveNumber, Waves.Num(), FormationClass ? *FormationClass->GetName() : TEXT("NULL"));

	if (!Waves.IsValidIndex(WaveNumber))
	{
		UE_LOG(LogTemp, Warning, TEXT("WaveManager: WaveNumber %d is out of range — aborting"), WaveNumber);
		return;
	}
	if (!FormationClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("WaveManager: FormationClass is NULL — aborting"));
		return;
	}

	// Tear down the previous formation
	if (IsValid(ActiveFormation))
	{
		UE_LOG(LogTemp, Log, TEXT("WaveManager: destroying previous formation '%s'"), *ActiveFormation->GetName());
		ActiveFormation->Destroy();
		ActiveFormation = nullptr;
	}

	WaveIndex = WaveNumber;

	// Apply per-wave scaling on top of the designer's base values
	FWaveConfig ScaledConfig         = Waves[WaveNumber];
	const float SpeedScale           = FMath::Pow(SpeedWaveScale,   WaveNumber);
	const float DescentScale         = FMath::Pow(DescentWaveScale, WaveNumber);
	const float IntervalScale        = FMath::Pow(0.9f,             WaveNumber);
	ScaledConfig.SpeedMultiplier    *= SpeedScale;
	ScaledConfig.DescentMultiplier  *= DescentScale;
	ScaledConfig.ShootIntervalMin   *= IntervalScale;
	ScaledConfig.ShootIntervalMax   *= IntervalScale;

	UE_LOG(LogTemp, Log, TEXT("WaveManager: wave %d — SpeedMultiplier=%.3f, DescentMultiplier=%.3f, ShootInterval=[%.2f, %.2f]"),
		WaveNumber, ScaledConfig.SpeedMultiplier, ScaledConfig.DescentMultiplier, ScaledConfig.ShootIntervalMin, ScaledConfig.ShootIntervalMax);

	ActiveFormation = GetWorld()->SpawnActorDeferred<AEnemyFormation>(
		FormationClass, GetActorTransform(), this);

	if (ActiveFormation)
	{
		ActiveFormation->Configure(ScaledConfig);
		UGameplayStatics::FinishSpawningActor(ActiveFormation, GetActorTransform());
		ActiveFormation->SetGameOverThreshold(GameOverXThreshold);
		ActiveFormation->OnFormationCleared.AddDynamic(this, &AWaveManager::OnFormationCleared);
		ActiveFormation->OnGameOver.AddDynamic(this, &AWaveManager::OnGameOverTriggered);
		UE_LOG(LogTemp, Log, TEXT("WaveManager: wave %d spawned — '%s'"), WaveNumber, *ActiveFormation->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("WaveManager: SpawnActorDeferred returned null for wave %d"), WaveNumber);
	}

	if (ASpaceInvaderGameState* GS = GetWorld()->GetGameState<ASpaceInvaderGameState>())
	{
		GS->SetWaveNumber(WaveNumber + 1); // wave numbers are 1-based in the UI
	}

	OnWaveStarted.Broadcast(WaveNumber);
}

void AWaveManager::OnFormationCleared()
{
	const int32 NextWave = WaveIndex + 1;
	UE_LOG(LogTemp, Log, TEXT("WaveManager: OnFormationCleared — current WaveIndex=%d, NextWave=%d, Waves.Num=%d"),
		WaveIndex, NextWave, Waves.Num());

	OnWaveCleared(WaveIndex);

	if (!Waves.IsValidIndex(NextWave))
	{
		UE_LOG(LogTemp, Log, TEXT("WaveManager: no more waves — all waves complete"));
		return;
	}

	PendingNextWave = NextWave;

	if (WaveClearedUIClass)
	{
		APlayerController* PC = GetWorld()->GetFirstPlayerController();
		UWaveClearedUI* Widget = CreateWidget<UWaveClearedUI>(PC, WaveClearedUIClass);
		if (Widget)
		{
			Widget->AddToViewport();
			Widget->OnCountdownFinished.AddDynamic(this, &AWaveManager::OnCountdownFinished);
			Widget->StartSequence();
			return;
		}
	}

	// Fallback: no widget class set — start immediately
	StartWave(PendingNextWave);
}

void AWaveManager::OnCountdownFinished()
{
	if (PendingNextWave >= 0)
	{
		StartWave(PendingNextWave);
		PendingNextWave = -1;
	}
}

void AWaveManager::OnGameOverTriggered()
{
	UE_LOG(LogTemp, Log, TEXT("WaveManager: Game Over — enemy crossed X threshold %.1f"), GameOverXThreshold);

	// Stop the formation so it doesn't keep advancing
	if (IsValid(ActiveFormation))
	{
		ActiveFormation->SetActorTickEnabled(false);
	}

	if (!WaveClearedUIClass) return;

	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	UWaveClearedUI* Widget = CreateWidget<UWaveClearedUI>(PC, WaveClearedUIClass);
	if (Widget)
	{
		Widget->AddToViewport();
		Widget->OnCountdownFinished.AddDynamic(this, &AWaveManager::OnRestartGame);
		Widget->StartGameOverSequence();
	}
}

void AWaveManager::OnRestartGame()
{
	UGameplayStatics::OpenLevel(this, FName(*UGameplayStatics::GetCurrentLevelName(this, true)));
}
