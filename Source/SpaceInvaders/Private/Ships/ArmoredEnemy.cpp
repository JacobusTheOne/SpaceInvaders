// Fill out your copyright notice in the Description page of Project Settings.

#include "Ships/ArmoredEnemy.h"
#include "Components/HealthComponent.h"

AArmoredEnemy::AArmoredEnemy()
{
	SetHealth(3);
	SetScoreValue(150);

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
}

void AArmoredEnemy::BeginPlay()
{
	Super::BeginPlay();
	UpdateWingMeshForTier(GetHealth());
}

float AArmoredEnemy::TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent,
	AController* EventInstigator, AActor* DamageCauser)
{
	float Applied = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (Applied > 0.f && !IsActorBeingDestroyed())
	{
		UpdateWingMeshForTier(GetHealth());
	}

	return Applied;
}

void AArmoredEnemy::UpdateWingMeshForTier(int32 CurrentHP)
{
	UMaterialInterface* Mat = nullptr;
	if      (CurrentHP >= 3) Mat = WingMaterial_Full;
	else if (CurrentHP == 2) Mat = WingMaterial_Damaged;
	else                     Mat = WingMaterial_Critical;

	if (MeshLeftWing)  MeshLeftWing->SetMaterial(0, Mat);
	if (MeshRightWing) MeshRightWing->SetMaterial(0, Mat);
}
