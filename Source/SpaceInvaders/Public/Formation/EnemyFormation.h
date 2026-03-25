// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyFormation.generated.h"

class AEnemyBase;
class UFormationMovementComponent;

UCLASS()
class SPACEINVADERS_API AEnemyFormation : public AActor
{
	GENERATED_BODY()

public:
	AEnemyFormation();

	const TArray<AEnemyBase*>& GetGrid() const { return Grid; }

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
	UPROPERTY(EditAnywhere, Category = "Formation")
	int32 Rows = 5;

	UPROPERTY(EditAnywhere, Category = "Formation")
	int32 Columns = 11;

	// World-space spacing between enemies
	UPROPERTY(EditAnywhere, Category = "Formation")
	float HorizontalSpacing = 120.f;

	UPROPERTY(EditAnywhere, Category = "Formation")
	float VerticalSpacing = 100.f;

	UPROPERTY(EditAnywhere, Category = "Formation")
	float StartingX = 254.688549f;

	UPROPERTY(EditAnywhere, Category = "Formation")
	float StartingY = -560.688549f;

	// Enemy class to spawn — set this to your Blueprint subclass
	UPROPERTY(EditAnywhere, Category = "Formation")
	TSubclassOf<AEnemyBase> EnemyClass;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UFormationMovementComponent* MovementComponent;
};
