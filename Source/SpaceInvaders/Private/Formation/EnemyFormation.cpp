// Fill out your copyright notice in the Description page of Project Settings.

#include "Formation/EnemyFormation.h"
#include "Ships/EnemyBase.h"
#include "Components/FormationMovementComponent.h"

void AEnemyFormation::Configure(const FWaveConfig& Config)
{
	EnemyClass        = Config.EnemyClass;
	Rows              = Config.EnemyRows;
	Columns           = Config.EnemyCols;
	HorizontalSpacing = Config.HorizontalSpacing;
	VerticalSpacing   = Config.VerticalSpacing;
	StartingX         = Config.StartingX;
	StartingY         = Config.StartingY;
	StartingZ         = Config.StartingZ;

	MovementComponent->MoveStep       = Config.MoveStep * Config.SpeedMultiplier;
	MovementComponent->StepInterval   = Config.StepInterval;
	MovementComponent->DropAmount     = Config.DropAmount * Config.DescentMultiplier;

	SpawnShootIntervalMin = Config.ShootIntervalMin;
	SpawnShootIntervalMax = Config.ShootIntervalMax;
}

AEnemyFormation::AEnemyFormation()
{
	PrimaryActorTick.bCanEverTick = false;

	MovementComponent = CreateDefaultSubobject<UFormationMovementComponent>(TEXT("MovementComponent"));
}

void AEnemyFormation::BeginPlay()
{
	Super::BeginPlay();
	SpawnGrid();
}

void AEnemyFormation::NotifyGameOver()
{
	OnGameOver.Broadcast();
}

void AEnemyFormation::SetGameOverThreshold(float Threshold)
{
	MovementComponent->PlayerXThreshold = Threshold;
}

void AEnemyFormation::OnEnemyDestroyed(AActor* DestroyedActor)
{
	const int32 Index = Grid.IndexOfByKey(Cast<AEnemyBase>(DestroyedActor));
	if (Index != INDEX_NONE)
	{
		Grid[Index] = nullptr;
	}

	if (AllDead())
	{
		UE_LOG(LogTemp, Log, TEXT("EnemyFormation: all enemies destroyed"));
		OnFormationCleared.Broadcast();
		OnAllEnemiesDestroyed();
	}
}

AEnemyBase* AEnemyFormation::GetBottommostInColumn(int32 Col) const
{
	for (int32 Row = 0; Row < Rows; ++Row)
	{
		AEnemyBase* Enemy = Grid[Row * Columns + Col];
		if (IsValid(Enemy))
		{
			return Enemy;
		}
	}
	return nullptr;
}

bool AEnemyFormation::AllDead() const
{
	for (AEnemyBase* Enemy : Grid)
	{
		if (IsValid(Enemy))
		{
			return false;
		}
	}
	return true;
}

void AEnemyFormation::SpawnGrid()
{
	if (!EnemyClass)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World) return;

	Grid.Empty();
	Grid.Reserve(Rows * Columns);

	const FVector Origin = FVector(StartingX, StartingY, GetActorLocation().Z);

	for (int32 Row = 0; Row < Rows; ++Row)
	{
		for (int32 Col = 0; Col < Columns; ++Col)
		{
			FVector SpawnLocation = Origin + FVector(
				Row * VerticalSpacing,    // X: rows step forward
				Col * HorizontalSpacing,  // Y: columns step right
				StartingZ
			);

			FActorSpawnParameters Params;
			Params.Owner = this;
			Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			AEnemyBase* Enemy = World->SpawnActor<AEnemyBase>(EnemyClass, SpawnLocation, FRotator::ZeroRotator, Params);

			if (Enemy)
			{
				Enemy->OnDestroyed.AddDynamic(this, &AEnemyFormation::OnEnemyDestroyed);
				Enemy->InitFormationData(this, Col);
				Enemy->SetFireInterval(SpawnShootIntervalMin, SpawnShootIntervalMax);
			}

			Grid.Add(Enemy);
		}
	}
}
