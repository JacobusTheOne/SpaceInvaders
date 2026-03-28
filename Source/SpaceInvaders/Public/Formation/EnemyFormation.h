// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Managers/WaveManager.h"
#include "EnemyFormation.generated.h"

class AEnemyBase;
class UFormationMovementComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFormationClearedSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameOverSignature);

UCLASS()
class SPACEINVADERS_API AEnemyFormation : public AActor
{
	GENERATED_BODY()

public:
	AEnemyFormation();

	const TArray<AEnemyBase*>& GetGrid() const { return Grid; }

	// Applied before BeginPlay via deferred spawn — sets grid dimensions, per-row enemy classes, and movement
	void Configure(const FWaveConfig& Config);

	// Returns the alive enemy in Col with the lowest row index (closest to the player)
	AEnemyBase* GetBottommostInColumn(int32 Col) const;

	// Broadcast when every enemy in the formation has been destroyed
	UPROPERTY(BlueprintAssignable, Category = "Formation")
	FOnFormationClearedSignature OnFormationCleared;

	// Broadcast when an enemy crosses the GameOver X threshold
	UPROPERTY(BlueprintAssignable, Category = "Formation")
	FOnGameOverSignature OnGameOver;

	// Called by FormationMovementComponent when the threshold is crossed
	void NotifyGameOver();

	// Sets the X threshold on the movement component (called by WaveManager after spawn)
	void SetGameOverThreshold(float Threshold);

	UFUNCTION(BlueprintImplementableEvent, Category = "Formation")
	void OnAllEnemiesDestroyed();

protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void OnEnemyDestroyed(AActor* DestroyedActor);

	bool AllDead() const;

private:
	void SpawnGrid();

	// 2D grid stored as a flat TArray — access via Grid[Row * Columns + Col]
	UPROPERTY()
	TArray<AEnemyBase*> Grid;

	// Grid dimensions
	UPROPERTY(EditAnywhere, Category = "Formation", meta = (AllowPrivateAccess = "true"))
	int32 Rows = 5;

	UPROPERTY(EditAnywhere, Category = "Formation", meta = (AllowPrivateAccess = "true"))
	int32 Columns = 11;
	 
	// World-space spacing between enemies
	UPROPERTY(EditAnywhere, Category = "Formation", meta = (AllowPrivateAccess = "true"))
	float HorizontalSpacing = 120.f;

	UPROPERTY(EditAnywhere, Category = "Formation", meta = (AllowPrivateAccess = "true"))
	float VerticalSpacing = 120.f;

	UPROPERTY(EditAnywhere, Category = "Formation", meta = (AllowPrivateAccess = "true"))
	float StartingX = 254.688549f;

	UPROPERTY(EditAnywhere, Category = "Formation", meta = (AllowPrivateAccess = "true"))
	float StartingY = -560.688549f;

	UPROPERTY(EditAnywhere, Category = "Formation", meta = (AllowPrivateAccess = "true"))
	float StartingZ = 108.f;

	// One class per row — filled by WaveManager::BuildWaveConfig via Configure().
	// Row 0 = back of formation (furthest from player), Row (Rows-1) = front.
	TArray<TSubclassOf<AEnemyBase>> RowClasses;

	// HP override per row (0 = use the Blueprint class default)
	TArray<int32> RowHealthOverrides;

	// Set via Configure — applied to each enemy on spawn
	float SpawnShootIntervalMin = 1.f;
	float SpawnShootIntervalMax = 4.f;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UFormationMovementComponent* MovementComponent;
};
