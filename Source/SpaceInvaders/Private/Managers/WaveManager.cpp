#include "Managers/WaveManager.h"
#include "Managers/SoundManager.h"
#include "Formation/EnemyFormation.h"
#include "GameState/SpaceInvaderGameState.h"
#include "UI/WaveClearedUI.h"
#include "Ships/BossEnemy.h"
#include "Components/Projectile.h"
#include "Kismet/GameplayStatics.h"
#include "Curves/CurveFloat.h"

AWaveManager::AWaveManager()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AWaveManager::BeginPlay()
{
	Super::BeginPlay();

	InitProjectilePool();

	if (bDebugBossOnly)
	{
		//UE_LOG(LogTemp, Warning, TEXT("WaveManager: bDebugBossOnly=true — skipping waves, spawning boss (invincible)"));
		PendingNextWave = -1;
		SpawnBoss();
		return;
	}

	// If a formation was placed in the level instead of spawned, bind to it and
	// configure it with wave 0 settings so it uses the procedural tier system.
	if (!IsValid(ActiveFormation))
	{
		ActiveFormation = Cast<AEnemyFormation>(
			UGameplayStatics::GetActorOfClass(GetWorld(), AEnemyFormation::StaticClass()));
	}

	if (IsValid(ActiveFormation))
	{
		//UE_LOG(LogTemp, Warning, TEXT("WaveManager: found existing formation '%s' — configuring with wave 0"), *ActiveFormation->GetName());
		ActiveFormation->Configure(BuildWaveConfig(0));
		ActiveFormation->SetGameOverThreshold(GameOverXThreshold);
		ActiveFormation->OnFormationCleared.AddDynamic(this, &AWaveManager::OnFormationCleared);
		ActiveFormation->OnGameOver.AddDynamic(this, &AWaveManager::OnGameOverTriggered);
	}
	else
	{
		//UE_LOG(LogTemp, Warning, TEXT("WaveManager: no existing formation found — calling StartWave(0)"));
		StartWave(0);
	}
}

// ── Procedural wave config ────────────────────────────────────────────────────

FWaveConfig AWaveManager::BuildWaveConfig(int32 WaveNumber) const
{
	FWaveConfig Config = BaseFormationConfig;

	Config.StartingY = FMath::RandRange(SpawnYMin, SpawnYMax);

	// Apply per-wave scaling on top of the designer's base values
	const float SpeedScale    = FMath::Pow(SpeedWaveScale,   WaveNumber);
	const float DescentScale  = FMath::Pow(DescentWaveScale, WaveNumber);
	const float IntervalScale = FMath::Pow(0.9f,             WaveNumber);
	Config.SpeedMultiplier   *= SpeedScale;
	Config.DescentMultiplier *= DescentScale;
	Config.StepInterval      *= Config.StepIntervalMultiplier;
	Config.ShootIntervalMin  *= IntervalScale * Config.ShootIntervalMinMultiplier;
	Config.ShootIntervalMax  *= IntervalScale * Config.ShootIntervalMaxMultiplier;

	// Compute difficulty progress [0, 1]:
	//   0 → all Tier-1 enemies   |   1 → all Tier-5 enemies
	float DifficultyT = 0.f;
	if (DifficultyProgressCurve)
	{
		DifficultyT = FMath::Clamp(DifficultyProgressCurve->GetFloatValue(static_cast<float>(WaveNumber)), 0.f, 1.f);
	}
	else
	{
		// Fallback: linear ramp that reaches Tier 5 by wave 20
		DifficultyT = FMath::Clamp(WaveNumber / 20.f, 0.f, 1.f);
	}

	// ── Assign a tier to each row ─────────────────────────────────────────────
	// Row 0 = back of formation (lowest threat), Row (Rows-1) = front (highest).
	// CenterTier = DifficultyT * 4  (maps [0,1] → [tier0, tier4]).
	// Each row is offset from the center by (NormalizedRow - 0.5) * TierSpread,
	// so TierSpread controls how much variety exists within a single wave.
	//
	//   TierSpread = 0  →  every row is identical  (pure difficulty ramp, no mixing)
	//   TierSpread = 2  →  front rows are 1 tier higher, back rows 1 tier lower
	//   TierSpread = 4  →  full spread from tier0 (back) to tier4 (front) at mid-game
	const int32 NumTiers = 5;
	const int32 NumRows  = Config.EnemyRows;
	const float CenterTier = DifficultyT * static_cast<float>(NumTiers - 1);

	// Scale spread by DifficultyT so mixing only opens up as the curve climbs.
	// At DifficultyT=0 every row is Tier 1; at DifficultyT=1 the full TierSpread applies.
	const float EffectiveSpread = TierSpread * DifficultyT;

	Config.RowClasses.SetNum(NumRows);
	Config.RowHealthOverrides.SetNum(NumRows);

	for (int32 Row = 0; Row < NumRows; ++Row)
	{
		const float NormalizedRow = (NumRows > 1) ? static_cast<float>(Row) / static_cast<float>(NumRows - 1) : 0.5f;
		const float TierFloat     = CenterTier + (NormalizedRow - 0.5f) * EffectiveSpread;
		const int32 TierIndex     = FMath::Clamp(FMath::RoundToInt(TierFloat), 0, NumTiers - 1);

		Config.RowClasses[Row]         = EnemyTiers[TierIndex].EnemyClass;
		Config.RowHealthOverrides[Row] = EnemyTiers[TierIndex].HealthOverride;
	}

	// ── Special enemy injection ───────────────────────────────────────────────
	++WavesSinceLastSpecial;
	if (SpecialEnemyClass && WaveNumber > 0 && WavesSinceLastSpecial >= NextSpecialWaveInterval)
	{
		Config.SpecialEnemyClass = SpecialEnemyClass;
		Config.SpecialEnemyRow   = FMath::RandRange(0, Config.EnemyRows - 1);
		Config.SpecialEnemyCol   = FMath::RandRange(0, Config.EnemyCols - 1);
		WavesSinceLastSpecial    = 0;
		NextSpecialWaveInterval  = FMath::RandRange(2, 3);
	}

	//UE_LOG(LogTemp, Warning, TEXT("WaveManager: wave %d — DifficultyT=%.3f, SpeedMult=%.3f, DescentMult=%.3f, ShootInterval=[%.2f, %.2f]"),
	//	WaveNumber, DifficultyT, Config.SpeedMultiplier, Config.DescentMultiplier, Config.ShootIntervalMin, Config.ShootIntervalMax);

	return Config;
}

// ── Wave lifecycle ────────────────────────────────────────────────────────────

void AWaveManager::StartWave(int32 WaveNumber)
{
	if (!FormationClass)
	{
		//UE_LOG(LogTemp, Warning, TEXT("WaveManager: FormationClass is NULL — aborting wave %d"), WaveNumber);
		return;
	}

	// Tear down the previous formation
	if (IsValid(ActiveFormation))
	{
		ActiveFormation->Destroy();
		ActiveFormation = nullptr;
	}

	WaveIndex = WaveNumber;

	ActiveFormation = GetWorld()->SpawnActorDeferred<AEnemyFormation>(
		FormationClass, GetActorTransform(), this);

	if (ActiveFormation)
	{
		ActiveFormation->Configure(BuildWaveConfig(WaveNumber));
		UGameplayStatics::FinishSpawningActor(ActiveFormation, GetActorTransform());
		ActiveFormation->SetGameOverThreshold(GameOverXThreshold);
		ActiveFormation->OnFormationCleared.AddDynamic(this, &AWaveManager::OnFormationCleared);
		ActiveFormation->OnGameOver.AddDynamic(this, &AWaveManager::OnGameOverTriggered);
		//UE_LOG(LogTemp, Warning, TEXT("WaveManager: wave %d spawned — '%s'"), WaveNumber, *ActiveFormation->GetName());
	}
	else
	{
		//UE_LOG(LogTemp, Error, TEXT("WaveManager: SpawnActorDeferred returned null for wave %d"), WaveNumber);
	}

	if (ASpaceInvaderGameState* GS = GetWorld()->GetGameState<ASpaceInvaderGameState>())
	{
		GS->SetWaveNumber(WaveNumber + 1); // 1-based in the UI
	}

	OnWaveStarted.Broadcast(WaveNumber);
}

void AWaveManager::OnFormationCleared()
{
	const int32 NextWave = WaveIndex + 1;
	//UE_LOG(LogTemp, Warning, TEXT("WaveManager: OnFormationCleared — WaveIndex=%d, starting wave %d"), WaveIndex, NextWave);

	OnWaveCleared(WaveIndex);

	PendingNextWave = NextWave;

	// Every 3rd wave (NextWave == 3, 6, 9 …) — spawn boss before next wave
	if (NextWave % 3 == 0)
	{
		if (BossClass)
		{
			SpawnBoss();
			return; // next wave starts in OnBossDestroyed
		}
		//UE_LOG(LogTemp, Warning, TEXT("WaveManager: wave %d is a boss wave but BossClass is not assigned — skipping boss"), NextWave);
	}

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

void AWaveManager::TriggerGameOver()
{
	OnGameOverTriggered();
}

void AWaveManager::OnGameOverTriggered()
{
	//UE_LOG(LogTemp, Warning, TEXT("WaveManager: Game Over — enemy crossed X threshold %.1f"), GameOverXThreshold);

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

// ── Boss ──────────────────────────────────────────────────────────────────────

void AWaveManager::SpawnBoss()
{
	FVector SpawnLoc = ABossEnemy::ClampToBounds(BossSpawnLocation, BossPatrolBoundsOrigin, BossPatrolBoundsExtent);
	const FTransform SpawnTransform(FRotator::ZeroRotator, SpawnLoc);

	ABossEnemy* Boss = GetWorld()->SpawnActorDeferred<ABossEnemy>(
		BossClass, SpawnTransform, this, nullptr,
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	if (Boss)
	{
		// Set bounds BEFORE FinishSpawningActor so BeginPlay sees the correct values
		Boss->bInvincible        = bDebugBossOnly;
		Boss->PatrolBoundsOrigin = BossPatrolBoundsOrigin;
		Boss->PatrolBoundsExtent = BossPatrolBoundsExtent;
		UGameplayStatics::FinishSpawningActor(Boss, SpawnTransform);
		Boss->OnDestroyed.AddDynamic(this, &AWaveManager::OnBossDestroyed);

		if (ASoundManager* SM = ASoundManager::Get(GetWorld()))
		{
			SM->PlayBossMusic();
		}
		//UE_LOG(LogTemp, Log, TEXT("WaveManager: boss spawned at %s before wave %d (invincible=%s)"), *SpawnLoc.ToString(), PendingNextWave, bDebugBossOnly ? TEXT("true") : TEXT("false"));
	}
	else
	{
		// SpawnActor failed (collision, invalid class, etc.) — don't leave the game stuck
		//UE_LOG(LogTemp, Error, TEXT("WaveManager: boss SpawnActor failed at %s — starting wave %d without boss"), *SpawnLoc.ToString(), PendingNextWave);
		StartWave(PendingNextWave);
		PendingNextWave = -1;
	}
}

void AWaveManager::OnBossDestroyed(AActor* DestroyedActor)
{
	//UE_LOG(LogTemp, Warning, TEXT("WaveManager: boss destroyed — starting wave %d"), PendingNextWave);

	if (ASoundManager* SM = ASoundManager::Get(GetWorld()))
	{
		SM->PlayGameplayMusic();
	}

	if (WaveClearedUIClass)
	{
		APlayerController* PC = GetWorld()->GetFirstPlayerController();
		UWaveClearedUI* Widget = CreateWidget<UWaveClearedUI>(PC, WaveClearedUIClass);
		if (Widget)
		{
			Widget->AddToViewport();
			Widget->OnCountdownFinished.AddDynamic(this, &AWaveManager::OnCountdownFinished);
			Widget->StartBossDestroyedSequence();
			return;
		}
	}

	// Fallback: no widget class set — start immediately
	if (PendingNextWave >= 0)
	{
		StartWave(PendingNextWave);
		PendingNextWave = -1;
	}
}

// ── Projectile pool ───────────────────────────────────────────────────────────

void AWaveManager::InitProjectilePool()
{
	if (!PooledProjectileClass) return;

	ProjectilePool.Reserve(ProjectilePoolSize);

	for (int32 i = 0; i < ProjectilePoolSize; ++i)
	{
		AProjectile* P = GetWorld()->SpawnActorDeferred<AProjectile>(
			PooledProjectileClass, FTransform::Identity, this);
		if (P)
		{
			P->bIsPooled = true;
			UGameplayStatics::FinishSpawningActor(P, FTransform::Identity);
			P->SetActorHiddenInGame(true);
			ProjectilePool.Add(P);
		}
	}

	//UE_LOG(LogTemp, Warning, TEXT("WaveManager: projectile pool initialised — %d/%d slots"),
		//ProjectilePool.Num(), ProjectilePoolSize);
}

AProjectile* AWaveManager::GetPooledProjectile(const FTransform& T, const FVector& Direction)
{
	for (AProjectile* P : ProjectilePool)
	{
		if (IsValid(P) && P->IsHidden())
		{
			P->Activate(T, Direction);
			return P;
		}
	}

	//UE_LOG(LogTemp, Warning, TEXT("WaveManager: projectile pool exhausted (max %d)"), ProjectilePoolSize);
	return nullptr;
}
