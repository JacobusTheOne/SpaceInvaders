// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Ships/EnemyBase.h"
#include "StandardEnemy.generated.h"

/**
 * 
 */
UCLASS()
class SPACEINVADERS_API AStandardEnemy : public AEnemyBase
{
	GENERATED_BODY()

public:
	AStandardEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
