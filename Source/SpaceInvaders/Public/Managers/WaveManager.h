#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WaveManager.generated.h"

class AEnemyBase;
class AEnemyFormation;
class ABossEnemy;
class AProjectile;
class UWaveClearedUI;
class UCurveFloat;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWaveStartedSignature, int32, WaveNumber);

// ── Enemy tier configuration ──────────────────────────────────────────────────
// Assign a Blueprint subclass and optional HP override for each of the 5 tiers.
USTRUCT(BlueprintType)
struct FEnemyTierConfig
{
	GENERATED_BODY()

	// Blueprint subclass to spawn for this tier
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tier")
	TSubclassOf<AEnemyBase> EnemyClass;

	// Override the class-default HP (0 = use the Blueprint default)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tier", meta = (ClampMin = "0"))
	int32 HealthOverride = 0;
};

// ── Per-wave formation configuration ─────────────────────────────────────────
// Layout and movement params shared across all waves.
// RowClasses / RowHealthOverrides are filled at runtime by AWaveManager::BuildWaveConfig
// and should not be set by hand.
USTRUCT(BlueprintType)
struct FWaveConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
	int32 EnemyRows = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
	int32 EnemyCols = 11;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
	float HorizontalSpacing = 120.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
	float VerticalSpacing = 100.f;

	// Formation movement
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
	float MoveStep = 40.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
	float StepInterval = 0.8f;

	// Scales MoveStep — multiplied by SpeedWaveScale^WaveIndex at runtime
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
	float SpeedMultiplier = 1.f;

	// Scales StepInterval — applied on top of the base value at runtime
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
	float StepIntervalMultiplier = 1.f;

	// How far (units) the formation drops each time it reverses
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
	float DropAmount = 80.f;

	// Scales DropAmount — multiplied by DescentWaveScale^WaveIndex at runtime
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
	float DescentMultiplier = 1.f;

	// Grid origin
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
	float StartingX = 254.688549f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
	float StartingY = -560.688549f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
	float StartingZ = 48.f;

	// Enemy fire timing — multiplied by 0.9^WaveIndex at runtime
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
	float ShootIntervalMin = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
	float ShootIntervalMax = 4.f;

	// Scale ShootIntervalMin/Max — applied on top of the per-wave interval scale
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
	float ShootIntervalMinMultiplier = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
	float ShootIntervalMaxMultiplier = 1.f;

	// ── Runtime-filled by BuildWaveConfig — not designer-editable ────────────
	// One entry per row; filled before passing to EnemyFormation::Configure.
	TArray<TSubclassOf<AEnemyBase>> RowClasses;
	TArray<int32>                   RowHealthOverrides;
};

UCLASS()
class SPACEINVADERS_API AWaveManager : public AActor
{
	GENERATED_BODY()

public:
	AWaveManager();

protected:
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable, Category = "Wave")
	void StartWave(int32 WaveNumber);

	UFUNCTION(BlueprintPure, Category = "Wave")
	int32 GetWaveIndex() const { return WaveIndex; }

	// Returns an inactive pooled projectile, fires it at Direction from T.
	AProjectile* GetPooledProjectile(const FTransform& T, const FVector& Direction);

	UPROPERTY(BlueprintAssignable, Category = "Wave")
	FOnWaveStartedSignature OnWaveStarted;

	UFUNCTION(BlueprintImplementableEvent, Category = "Wave")
	void OnWaveCleared(int32 ClearedWaveIndex);

	// ── Debug ─────────────────────────────────────────────────────────────────
	// When true: skips all waves at BeginPlay and spawns only the boss (invincible).
	// Hitbox hits are logged but deal no damage, so the boss stays alive indefinitely.
	UPROPERTY(EditAnywhere, Category = "Debug")
	bool bDebugBossOnly = false;

private:
	UFUNCTION()
	void OnFormationCleared();

	UFUNCTION()
	void OnCountdownFinished();

	UFUNCTION()
	void OnGameOverTriggered();

	UFUNCTION()
	void OnRestartGame();

	UFUNCTION()
	void OnBossDestroyed(AActor* DestroyedActor);

	void InitProjectilePool();
	void SpawnBoss();

	// Procedurally builds a complete FWaveConfig for WaveNumber, including
	// RowClasses assigned from EnemyTiers using the DifficultyProgressCurve.
	FWaveConfig BuildWaveConfig(int32 WaveNumber) const;

private:
	// ── Enemy tier setup ─────────────────────────────────────────────────────
	// Five tiers ordered weakest → strongest. Assign Blueprint subclasses and
	// optional HP overrides here. Wave 0 uses only Tier 1; waves scale toward
	// all Tier 5 enemies according to DifficultyProgressCurve.
	UPROPERTY(EditAnywhere, Category = "Wave|EnemyTiers", meta = (AllowPrivateAccess = "true"))
	FEnemyTierConfig EnemyTiers[5];

	// ── Difficulty progression ────────────────────────────────────────────────
	// Curve that maps wave number (X) to a difficulty value in [0, 1] (Y).
	//   Y = 0  →  all enemies are Tier 1
	//   Y = 1  →  all enemies are Tier 5
	// If not assigned, a linear ramp over 20 waves is used as a fallback.
	UPROPERTY(EditAnywhere, Category = "Wave|Progression", meta = (AllowPrivateAccess = "true"))
	UCurveFloat* DifficultyProgressCurve = nullptr;

	// Controls tier variety within a single wave.
	//   0 = every row uses the same tier (pure progression, no mixing).
	//   4 = front rows are up to 2 tiers higher and back rows up to 2 tiers
	//       lower than the wave's average tier (maximum in-wave mixing).
	// Recommended starting value: 2
	UPROPERTY(EditAnywhere, Category = "Wave|Progression", meta = (ClampMin = "0.0", ClampMax = "4.0", AllowPrivateAccess = "true"))
	float TierSpread = 2.f;

	// ── Formation layout / movement defaults ──────────────────────────────────
	// These values are used as the base for every wave before per-wave scaling
	// is applied. EnemyClass is no longer used here — tiers are assigned above.
	UPROPERTY(EditAnywhere, Category = "Wave|Layout", meta = (AllowPrivateAccess = "true"))
	FWaveConfig BaseFormationConfig;

	// Formation Blueprint subclass to spawn each wave
	UPROPERTY(EditAnywhere, Category = "Wave", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AEnemyFormation> FormationClass;

	UPROPERTY(EditAnywhere, Category = "Wave", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UWaveClearedUI> WaveClearedUIClass;

	// X coordinate at which an enemy triggers Game Over (world units)
	UPROPERTY(EditAnywhere, Category = "Wave|GameOver", meta = (AllowPrivateAccess = "true"))
	float GameOverXThreshold = 0.f;

	// Per-wave multiplier for descent speed (e.g. 1.15 = 15% faster per wave)
	UPROPERTY(EditAnywhere, Category = "Wave|Scaling", meta = (AllowPrivateAccess = "true"))
	float DescentWaveScale = 1.15f;

	// Per-wave multiplier for lateral movement speed
	UPROPERTY(EditAnywhere, Category = "Wave|Scaling", meta = (AllowPrivateAccess = "true"))
	float SpeedWaveScale = 1.15f;

	UPROPERTY(VisibleInstanceOnly, Category = "Wave", meta = (AllowPrivateAccess = "true"))
	AEnemyFormation* ActiveFormation = nullptr;

	UPROPERTY(VisibleInstanceOnly, Category = "Wave", meta = (AllowPrivateAccess = "true"))
	int32 WaveIndex = 0;

	int32 PendingNextWave = -1;

	// Boss spawns every 3rd cleared wave (after waves 3, 6, 9 …)
	UPROPERTY(EditAnywhere, Category = "Wave|Boss", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<ABossEnemy> BossClass;

	// World-space location where the boss actor is spawned
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave|Boss", meta = (AllowPrivateAccess = "true"))
	FVector BossSpawnLocation = FVector(200.f, 0.f, 48.f);

	// X/Y world-space centre of the boss patrol area
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave|Boss", meta = (AllowPrivateAccess = "true"))
	FVector2D BossPatrolBoundsOrigin = FVector2D(200.f, 0.f);

	// X/Y half-extents of the boss patrol area
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave|Boss", meta = (AllowPrivateAccess = "true"))
	FVector2D BossPatrolBoundsExtent = FVector2D(50.f, 400.f);

	// Projectile class pre-spawned into the pool — assign in Blueprint
	UPROPERTY(EditAnywhere, Category = "Wave|Pool", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AProjectile> PooledProjectileClass;

	static constexpr int32 ProjectilePoolSize = 60;
	TArray<TObjectPtr<AProjectile>> ProjectilePool;
};
