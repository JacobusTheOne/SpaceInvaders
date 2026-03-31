// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Ships/EnemyBase.h"
#include "BossEnemy.generated.h"

class AWaveManager;
class UBossSFXComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBossHealthChanged, int32, CurrentHealth, int32, MaxHealth);

UENUM(BlueprintType)
enum class EBossPhase : uint8
{
	Idle     UMETA(DisplayName = "Idle"),
	Attack   UMETA(DisplayName = "Attack"),
	Enraged  UMETA(DisplayName = "Enraged"),
};

UCLASS()
class SPACEINVADERS_API ABossEnemy : public AEnemyBase
{
	GENERATED_BODY()

public:
	ABossEnemy();

	UPROPERTY(BlueprintAssignable, Category = "Boss|Health")
	FOnBossHealthChanged OnHealthChanged;

	UFUNCTION(BlueprintPure, Category = "Boss|Health")
	int32 GetMaxHealth() const { return MaxHealth; }

	// Set to true by WaveManager debug tools — boss takes no damage but hitboxes still register
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	bool bInvincible = false;

	virtual bool ShouldDieOnPlayerContact() const override { return false; }

	// Returns DesiredLocation with its X and Y clamped inside the given patrol bounds,
	// preserving Z. Use this before spawning to guarantee the boss starts within its area.
	static FVector ClampToBounds(const FVector& DesiredLocation,
	                              const FVector2D& BoundsOrigin,
	                              const FVector2D& BoundsExtent);

	// Set by WaveManager before BeginPlay — X/Y world-space centre of the patrol area.
	UPROPERTY(BlueprintReadWrite, Category = "Boss|Movement")
	FVector2D PatrolBoundsOrigin = FVector2D(200.f, 0.f);

	// Set by WaveManager before BeginPlay — X/Y half-extents of the patrol area.
	UPROPERTY(BlueprintReadWrite, Category = "Boss|Movement")
	FVector2D PatrolBoundsExtent = FVector2D(50.f, 400.f);

	UFUNCTION(BlueprintPure, Category = "Boss")
	EBossPhase GetPhase() const { return CurrentPhase; }

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	virtual float TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent,
		AController* EventInstigator, AActor* DamageCauser) override;

	// Angle (degrees) between each of the 3 spread projectiles
	UPROPERTY(EditDefaultsOnly, Category = "Boss|Combat", meta = (ClampMin = "1.0", ClampMax = "45.0", AllowProtectedAccess = "true"))
	float SpreadAngleDeg = 20.f;

	// Speed of the charge (units/second)
	UPROPERTY(EditDefaultsOnly, Category = "Boss|Combat", meta = (ClampMin = "100.0", AllowProtectedAccess = "true"))
	float ChargeSpeed = 800.f;

	// Total charge duration — boss moves forward for half, retreats for half
	UPROPERTY(EditDefaultsOnly, Category = "Boss|Combat", meta = (ClampMin = "0.2", AllowProtectedAccess = "true"))
	float ChargeDuration = 2.f;

	// Seconds between attacks in the Attack phase
	UPROPERTY(EditDefaultsOnly, Category = "Boss|Combat", meta = (ClampMin = "0.1", AllowProtectedAccess = "true"))
	float AttackInterval = 2.f;

	// Seconds between attacks in the Enraged phase
	UPROPERTY(EditDefaultsOnly, Category = "Boss|Combat", meta = (ClampMin = "0.1", AllowProtectedAccess = "true"))
	float EnragedAttackInterval = 1.f;

	// Speed of spread-shot projectiles (0 = use pool default)
	UPROPERTY(EditDefaultsOnly, Category = "Boss|Combat", meta = (ClampMin = "0.0", AllowProtectedAccess = "true"))
	float SpreadShotSpeed = 0.f;

	// Speed of aimed-shot projectiles (0 = use pool default)
	UPROPERTY(EditDefaultsOnly, Category = "Boss|Combat", meta = (ClampMin = "0.0", AllowProtectedAccess = "true"))
	float AimedShotSpeed = 0.f;

	UFUNCTION(BlueprintCallable, Category = "Boss|Combat")
	void FireSpreadShot();   // 3 projectiles fanned in the Y plane

	UFUNCTION(BlueprintCallable, Category = "Boss|Combat")
	void FireAimedShot();    // single shot directed at the player's world position

	// ── Patrol movement ───────────────────────────────────────────────────────
	// Speed (units/second) the boss moves between patrol points.
	UPROPERTY(EditDefaultsOnly, Category = "Boss|Movement", meta = (ClampMin = "10.0", AllowProtectedAccess = "true"))
	float PatrolMoveSpeed = 200.f;

	// Pause time (seconds) when the boss reaches a patrol point.
	UPROPERTY(EditDefaultsOnly, Category = "Boss|Movement", meta = (ClampMin = "0.0", AllowProtectedAccess = "true"))
	float PatrolWaitTime = 0.5f;

private:
	void EnterPhase(EBossPhase NewPhase);
	void ScheduleNextAttack();
	void ExecuteNextAttack();

	void StartCharge();      // lunge toward player, then retreat
	void EndCharge();

	void  GeneratePatrolPoints();
	FVector RandomPointInBounds() const;

	int32 MaxHealth           = 0;
	EBossPhase CurrentPhase   = EBossPhase::Idle;
	int32 AttackCycleIndex    = 0;

	bool    bIsCharging       = false;
	float   ChargeElapsed     = 0.f;
	FVector ChargeDirection   = FVector::ZeroVector;
	FVector ChargeOrigin      = FVector::ZeroVector;

	// Three patrol destinations — current target is PatrolPoints[PatrolTargetIndex].
	// The visited point is re-randomised each time the boss arrives, keeping movement fresh.
	TArray<FVector> PatrolPoints;
	int32  PatrolTargetIndex   = 0;
	float  PatrolWaitRemaining = 0.f;
	float  PatrolSpawnZ        = 0.f; // Z captured at spawn — held constant during patrol

	FTimerHandle AttackTimer;

	TWeakObjectPtr<AWaveManager> CachedWaveManager;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UBossSFXComponent* BossSFXComponent;
};
