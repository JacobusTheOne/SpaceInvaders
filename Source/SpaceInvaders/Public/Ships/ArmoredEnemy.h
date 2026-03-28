// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Ships/EnemyBase.h"
#include "ArmoredEnemy.generated.h"

class UHealthComponent;

UCLASS()
class SPACEINVADERS_API AArmoredEnemy : public AEnemyBase
{
	GENERATED_BODY()

public:
	AArmoredEnemy();

protected:
	virtual void BeginPlay() override;

	virtual float TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent,
		AController* EventInstigator, AActor* DamageCauser) override;

	// Assign one material per HP tier in the Blueprint defaults
	UPROPERTY(EditDefaultsOnly, Category = "ArmoredEnemy|Wings", meta = (AllowProtectedAccess = "true"))
	TObjectPtr<UMaterialInterface> WingMaterial_Full;     // 3 HP

	UPROPERTY(EditDefaultsOnly, Category = "ArmoredEnemy|Wings", meta = (AllowProtectedAccess = "true"))
	TObjectPtr<UMaterialInterface> WingMaterial_Damaged;  // 2 HP

	UPROPERTY(EditDefaultsOnly, Category = "ArmoredEnemy|Wings", meta = (AllowProtectedAccess = "true"))
	TObjectPtr<UMaterialInterface> WingMaterial_Critical; // 1 HP

	// Overrides the base Health int — set MaxHealth = 3 in Blueprint defaults
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UHealthComponent> HealthComponent;

private:
	void UpdateWingMeshForTier(int32 CurrentHP);
};
