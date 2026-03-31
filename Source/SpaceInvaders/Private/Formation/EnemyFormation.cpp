// Fill out your copyright notice in the Description page of Project Settings.

#include "Formation/EnemyFormation.h"
#include "Ships/EnemyBase.h"
#include "Components/FormationMovementComponent.h"

void AEnemyFormation::Configure(const FWaveConfig& Config)
{
	RowClasses         = Config.RowClasses;
	RowHealthOverrides = Config.RowHealthOverrides;
	Rows               = Config.EnemyRows;
	Columns            = Config.EnemyCols;
	HorizontalSpacing  = Config.HorizontalSpacing;
	VerticalSpacing    = Config.VerticalSpacing;
	StartingX          = Config.StartingX;
	StartingY          = Config.StartingY;
	StartingZ          = Config.StartingZ;

	MovementComponent->MoveStep     = Config.MoveStep * Config.SpeedMultiplier;
	MovementComponent->StepInterval = Config.StepInterval;
	MovementComponent->DropAmount   = Config.DropAmount * Config.DescentMultiplier;

	SpawnShootIntervalMin = Config.ShootIntervalMin;
	SpawnShootIntervalMax = Config.ShootIntervalMax;

	SpecialEnemyClass = Config.SpecialEnemyClass;
	SpecialEnemyRow   = Config.SpecialEnemyRow;
	SpecialEnemyCol   = Config.SpecialEnemyCol;

	// If BeginPlay has already run (level-placed formation), spawn the grid now.
	// For deferred-spawned formations Configure is called before BeginPlay,
	// so BeginPlay handles the spawn instead.
	if (HasActorBegunPlay())
	{
		SpawnGrid();
	}
}

AEnemyFormation::AEnemyFormation()
{
	PrimaryActorTick.bCanEverTick = false;

	MovementComponent = CreateDefaultSubobject<UFormationMovementComponent>(TEXT("MovementComponent"));
}

void AEnemyFormation::BeginPlay()
{
	Super::BeginPlay();
	// Only spawn here for deferred-spawned formations (Configure already called).
	// Level-placed formations receive Configure after BeginPlay, so SpawnGrid
	// is triggered from Configure instead.
	if (!RowClasses.IsEmpty())
	{
		SpawnGrid();
	}
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
	if (RowClasses.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("EnemyFormation: RowClasses is empty — no enemies will spawn"));
		return;
	}

	UWorld* World = GetWorld();
	if (!World) return;

	Grid.Empty();
	Grid.Reserve(Rows * Columns);

	const FVector Origin = FVector(StartingX, StartingY, StartingZ);

	for (int32 Row = 0; Row < Rows; ++Row)
	{
		// Use the class assigned to this row; fall back to the first row's class if out of range
		TSubclassOf<AEnemyBase> ClassForRow = RowClasses.IsValidIndex(Row) ? RowClasses[Row] : RowClasses[0];
		const int32 HealthOverride          = RowHealthOverrides.IsValidIndex(Row) ? RowHealthOverrides[Row] : 0;

		if (!ClassForRow)
		{
			// Fill the grid slots so indices stay correct even if a tier has no class assigned
			for (int32 Col = 0; Col < Columns; ++Col)
			{
				Grid.Add(nullptr);
			}
			continue;
		}

		for (int32 Col = 0; Col < Columns; ++Col)
		{
			const FVector SpawnLocation = Origin + FVector(
				Row * VerticalSpacing,    // X: rows step forward
				Col * HorizontalSpacing,  // Y: columns step right
				0.f
			);

			FActorSpawnParameters Params;
			Params.Owner = this;
			Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			// Use the special enemy class for the designated slot, if set
			TSubclassOf<AEnemyBase> ActualClass = (SpecialEnemyClass && Row == SpecialEnemyRow && Col == SpecialEnemyCol)
				? SpecialEnemyClass
				: ClassForRow;

			AEnemyBase* Enemy = World->SpawnActor<AEnemyBase>(ActualClass, SpawnLocation, FRotator::ZeroRotator, Params);

			if (Enemy)
			{
				if (HealthOverride > 0)
				{
					Enemy->SetHealth(HealthOverride);
				}
				Enemy->OnDestroyed.AddDynamic(this, &AEnemyFormation::OnEnemyDestroyed);
				Enemy->InitFormationData(this, Col);
				Enemy->SetFireInterval(SpawnShootIntervalMin, SpawnShootIntervalMax);
			}

			Grid.Add(Enemy);
		}
	}
}
