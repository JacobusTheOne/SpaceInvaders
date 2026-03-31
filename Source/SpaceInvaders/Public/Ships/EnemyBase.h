// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "EnemyBase.generated.h"

class UBoxComponent;
class USceneComponent;
class UShootingComponent;
class AProjectile;
class AEnemyFormation;
class UEnemySFXComponent;
class UNiagaraSystem;

UCLASS()
class SPACEINVADERS_API AEnemyBase : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AEnemyBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UEnemySFXComponent* SFXComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Enemy|VFX")
	UNiagaraSystem* ExplosionEffect;

	// Random fire interval range (seconds)
	UPROPERTY(EditDefaultsOnly, Category = "Enemy|Shooting")
	float MinFireInterval = 1.f;

	UPROPERTY(EditDefaultsOnly, Category = "Enemy|Shooting")
	float MaxFireInterval = 4.f;

	// Projectile overrides — 0 means use the projectile class default
	UPROPERTY(EditDefaultsOnly, Category = "Enemy|Projectile", meta = (ClampMin = "0.0"))
	float ProjectileInitialSpeed = 0.f;

	UPROPERTY(EditDefaultsOnly, Category = "Enemy|Projectile", meta = (ClampMin = "0.0"))
	float ProjectileMaxSpeed = 0.f;

	UPROPERTY(EditDefaultsOnly, Category = "Enemy|Projectile", meta = (ClampMin = "0.0"))
	float ProjectileLifeSpan = 0.f;

	UPROPERTY(EditDefaultsOnly, Category = "Enemy|Projectile", meta = (ClampMin = "0.0"))
	float ProjectileDamage = 0.f;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UShootingComponent* ShootingComponent;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual float TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent,
		AController* EventInstigator, AActor* DamageCauser) override;

	// Returns false for enemies (e.g. the boss) that should survive player contact
	virtual bool ShouldDieOnPlayerContact() const { return true; }

	// Getters
	UFUNCTION(BlueprintPure, Category = "Enemy")
	int32 GetHealth() const { return Health; }

	UFUNCTION(BlueprintPure, Category = "Enemy")
	int32 GetScoreValue() const { return ScoreValue; }

	// Setters
	UFUNCTION(BlueprintCallable, Category = "Enemy")
	void SetHealth(int32 NewHealth) { Health = NewHealth; }

	UFUNCTION(BlueprintCallable, Category = "Enemy")
	void SetScoreValue(int32 NewScoreValue) { ScoreValue = NewScoreValue; }

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Enemy")
	int32 Health;
	UPROPERTY(EditDefaultsOnly, Category = "Enemy")
	int32 ScoreValue;
public:
	void InitFormationData(AEnemyFormation* InFormation, int32 InColumnIndex);
	void SetFireInterval(float Min, float Max) { MinFireInterval = Min; MaxFireInterval = Max; }

private:
	TWeakObjectPtr<AEnemyFormation> Formation;
	int32 ColumnIndex = INDEX_NONE;

	FTimerHandle FireIntervalTimer;

	void FireAndReschedule();
	void ScheduleNextShot();

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USceneComponent* SceneRoot;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UBoxComponent* CollisionComponent;

protected:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* MeshHull;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* MeshCockpit;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* MeshLeftWing;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* MeshRightWing;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* MeshLeftGun;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* MeshRightGun;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* MeshEngine;

private:


};
