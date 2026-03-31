// Fill out your copyright notice in the Description page of Project Settings.

#include "Ships/BossEnemy.h"
#include "Components/BossSFXComponent.h"
#include "Managers/WaveManager.h"
#include "Components/Projectile.h"
#include "Kismet/GameplayStatics.h"
//#include "DrawDebugHelpers.h"

ABossEnemy::ABossEnemy()
{
	PrimaryActorTick.bCanEverTick = true;
	SetHealth(3);
	SetScoreValue(1000);

	BossSFXComponent = CreateDefaultSubobject<UBossSFXComponent>(TEXT("BossSFXComponent"));
}

void ABossEnemy::BeginPlay()
{
	Super::BeginPlay();

	// Snapshot health after Blueprint CDO values are applied — this becomes the bar's 100%
	MaxHealth = GetHealth();

	CachedWaveManager = Cast<AWaveManager>(
		UGameplayStatics::GetActorOfClass(GetWorld(), AWaveManager::StaticClass()));

	GeneratePatrolPoints();
	EnterPhase(EBossPhase::Attack);
}

void ABossEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//const FVector BoundsCenter(PatrolBoundsOrigin.X, PatrolBoundsOrigin.Y, PatrolSpawnZ);
	//const FVector BoundsExtent(PatrolBoundsExtent.X, PatrolBoundsExtent.Y, 10.f);
	//DrawDebugBox(GetWorld(), BoundsCenter, BoundsExtent, FColor::Cyan, false, -1.f, 0, 3.f);

	if (bIsCharging)
	{
		ChargeElapsed += DeltaTime;

		const FVector Dir = (ChargeElapsed < ChargeDuration * 0.5f) ? ChargeDirection : -ChargeDirection;
		AddActorWorldOffset(Dir * ChargeSpeed * DeltaTime, false);

		if (ChargeElapsed >= ChargeDuration)
		{
			EndCharge();
		}
		return;
	}

	// ── Patrol movement ───────────────────────────────────────────────────────
	if (PatrolPoints.Num() < 3) return;

	if (PatrolWaitRemaining > 0.f)
	{
		PatrolWaitRemaining -= DeltaTime;
		return;
	}

	const FVector Target = PatrolPoints[PatrolTargetIndex];
	const FVector NewLoc = FMath::VInterpConstantTo(GetActorLocation(), Target, DeltaTime, PatrolMoveSpeed);
	SetActorLocation(NewLoc);

	if (FVector::Dist(NewLoc, Target) < 5.f)
	{
		// Replace the visited point with a new random one, then advance
		PatrolPoints[PatrolTargetIndex] = RandomPointInBounds();
		PatrolTargetIndex = (PatrolTargetIndex + 1) % 3;
		PatrolWaitRemaining = PatrolWaitTime;
	}
}

float ABossEnemy::TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent,
	AController* EventInstigator, AActor* DamageCauser)
{
	if (bInvincible)
	{
		//UE_LOG(LogTemp, Log, TEXT("BossEnemy: hit registered on '%s' (invincible — no damage applied)"), *GetName());
		return 0.f;
	}

	float Applied = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (Applied > 0.f && !IsActorBeingDestroyed())
	{
		OnHealthChanged.Broadcast(GetHealth(), MaxHealth);
		BossSFXComponent->PlayHit();

		if (GetHealth() <= 1 && CurrentPhase != EBossPhase::Enraged)
		{
			EnterPhase(EBossPhase::Enraged);
		}
	}

	return Applied;
}

// ── Phase ────────────────────────────────────────────────────────────────────

void ABossEnemy::EnterPhase(EBossPhase NewPhase)
{
	CurrentPhase     = NewPhase;
	BossSFXComponent->PlayPhaseChange();
	AttackCycleIndex = 0;
	GetWorldTimerManager().ClearTimer(AttackTimer);
	ScheduleNextAttack();
}

void ABossEnemy::ScheduleNextAttack()
{
	if (CurrentPhase == EBossPhase::Idle) return;

	const float Interval = (CurrentPhase == EBossPhase::Enraged) ? EnragedAttackInterval : AttackInterval;
	GetWorldTimerManager().SetTimer(AttackTimer, this, &ABossEnemy::ExecuteNextAttack, Interval, false);
}

void ABossEnemy::ExecuteNextAttack()
{
	if (bIsCharging)
	{
		ScheduleNextAttack();
		return;
	}

	// Cycle: SpreadShot → AimedShot → Charge (replaced by AimedShot when Enraged)
	switch (AttackCycleIndex % 3)
	{
		case 0: FireSpreadShot(); break;
		case 1: FireAimedShot();  break;
		case 2:
			if (CurrentPhase == EBossPhase::Enraged) FireAimedShot();
			else                                     StartCharge();
			break;
	}

	++AttackCycleIndex;
	ScheduleNextAttack();
}

// ── Attack patterns ──────────────────────────────────────────────────────────

void ABossEnemy::FireSpreadShot()
{
	if (!CachedWaveManager.IsValid()) return;

	const FVector Origin = GetActorLocation();

	// Three projectiles: left, centre, right — spread in the Y plane
	for (int32 i = -1; i <= 1; ++i)
	{
		const FVector Dir = FRotator(0.f, i * SpreadAngleDeg, 0.f)
		                        .RotateVector(FVector(-1.f, 0.f, 0.f));
		if (AProjectile* P = CachedWaveManager->GetPooledProjectile(FTransform(Dir.Rotation(), Origin), Dir))
		{
			if (SpreadShotSpeed > 0.f) P->OverrideSpeed(SpreadShotSpeed);
		}
	}

	BossSFXComponent->PlaySpreadShot();
}

void ABossEnemy::FireAimedShot()
{
	if (!CachedWaveManager.IsValid()) return;

	const APawn* Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (!Player) return;

	const FVector Origin = GetActorLocation();
	const FVector Dir    = (Player->GetActorLocation() - Origin).GetSafeNormal();
	if (AProjectile* P = CachedWaveManager->GetPooledProjectile(FTransform(Dir.Rotation(), Origin), Dir))
	{
		if (AimedShotSpeed > 0.f) P->OverrideSpeed(AimedShotSpeed);
		BossSFXComponent->PlayAimedShot();
	}
}

void ABossEnemy::StartCharge()
{
	const APawn* Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (!Player) return;

	bIsCharging      = true;
	ChargeElapsed    = 0.f;
	ChargeOrigin     = GetActorLocation();
	ChargeDirection  = (Player->GetActorLocation() - ChargeOrigin).GetSafeNormal();

	BossSFXComponent->PlayChargeStart();
}

void ABossEnemy::EndCharge()
{
	bIsCharging = false;
	SetActorLocation(ChargeOrigin); // snap back cleanly
	BossSFXComponent->PlayChargeEnd();
}

// ── Static helpers ────────────────────────────────────────────────────────────

FVector ABossEnemy::ClampToBounds(const FVector& DesiredLocation,
                                   const FVector2D& BoundsOrigin,
                                   const FVector2D& BoundsExtent)
{
	return FVector(
		FMath::Clamp(DesiredLocation.X, BoundsOrigin.X - BoundsExtent.X, BoundsOrigin.X + BoundsExtent.X),
		FMath::Clamp(DesiredLocation.Y, BoundsOrigin.Y - BoundsExtent.Y, BoundsOrigin.Y + BoundsExtent.Y),
		DesiredLocation.Z
	);
}

// ── Patrol helpers ────────────────────────────────────────────────────────────

FVector ABossEnemy::RandomPointInBounds() const
{
	return FVector(
		PatrolBoundsOrigin.X + FMath::RandRange(-PatrolBoundsExtent.X, PatrolBoundsExtent.X),
		PatrolBoundsOrigin.Y + FMath::RandRange(-PatrolBoundsExtent.Y, PatrolBoundsExtent.Y),
		PatrolSpawnZ
	);
}

void ABossEnemy::GeneratePatrolPoints()
{
	PatrolSpawnZ = GetActorLocation().Z;
	PatrolPoints.SetNum(3);
	for (FVector& Point : PatrolPoints)
	{
		Point = RandomPointInBounds();
	}
	PatrolTargetIndex   = 0;
	PatrolWaitRemaining = 0.f;
}
