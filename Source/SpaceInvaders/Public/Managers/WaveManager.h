#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WaveManager.generated.h"

class AEnemyBase;
class AEnemyFormation;
class UWaveClearedUI;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWaveStartedSignature, int32, WaveNumber);

USTRUCT(BlueprintType)
struct FWaveConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
	TSubclassOf<AEnemyBase> EnemyClass;

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

	// How far (X units) the formation drops each time it reverses
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

	// Broadcast when a new wave begins — bind to this from C++ or Blueprint
	UPROPERTY(BlueprintAssignable, Category = "Wave")
	FOnWaveStartedSignature OnWaveStarted;

	UFUNCTION(BlueprintImplementableEvent, Category = "Wave")
	void OnWaveCleared(int32 ClearedWaveIndex);

private:
	UFUNCTION()
	void OnFormationCleared();

	UFUNCTION()
	void OnCountdownFinished();

	UFUNCTION()
	void OnGameOverTriggered();

	UFUNCTION()
	void OnRestartGame();

private:
	// Per-wave configuration — fill these out in the Blueprint Class Defaults
	UPROPERTY(EditAnywhere, Category = "Wave")
	TArray<FWaveConfig> Waves;

	// Formation Blueprint subclass to spawn each wave
	UPROPERTY(EditAnywhere, Category = "Wave")
	TSubclassOf<AEnemyFormation> FormationClass;

	UPROPERTY(EditAnywhere, Category = "Wave")
	TSubclassOf<UWaveClearedUI> WaveClearedUIClass;

	// X coordinate at which an enemy triggers Game Over (world units)
	UPROPERTY(EditAnywhere, Category = "Wave|GameOver")
	float GameOverXThreshold = 0.f;

	// Per-wave multiplier applied to DescentMultiplier each wave (e.g. 1.15 = 15% faster drop per wave)
	UPROPERTY(EditAnywhere, Category = "Wave|GameOver")
	float DescentWaveScale = 1.15f;

	// Per-wave multiplier applied to SpeedMultiplier each wave (e.g. 1.15 = 15% faster lateral movement per wave)
	UPROPERTY(EditAnywhere, Category = "Wave")
	float SpeedWaveScale = 1.15f;

	UPROPERTY(VisibleInstanceOnly, Category = "Wave")
	AEnemyFormation* ActiveFormation = nullptr;

	UPROPERTY(VisibleInstanceOnly, Category = "Wave")
	int32 WaveIndex = 0;

	int32 PendingNextWave = -1;
};
