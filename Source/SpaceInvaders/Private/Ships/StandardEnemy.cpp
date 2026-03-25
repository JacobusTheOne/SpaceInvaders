// Fill out your copyright notice in the Description page of Project Settings.


#include "Ships/StandardEnemy.h"

AStandardEnemy::AStandardEnemy()
{
	SetScoreValue(50);
	SetHealth(1);
}

void AStandardEnemy::BeginPlay()
{
	Super::BeginPlay();
}

void AStandardEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
