// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/ShootingComponent.h"
#include "Components/Projectile.h"

UShootingComponent::UShootingComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void UShootingComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}



void UShootingComponent::TryFire()
{
	if (!bCanFire || !ProjectileClass) return;

	AActor* Owner = GetOwner();
	if (!Owner) return;

	UWorld* World = GetWorld();
	if (!World) return;

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = Owner;
	SpawnParams.Instigator = Owner->GetInstigator();
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	World->SpawnActor<AProjectile>(ProjectileClass, GetComponentLocation(), FireDirection.Rotation(), SpawnParams);

	StartCooldown();
}

void UShootingComponent::StartCooldown()
{
	bCanFire = false;
	GetWorld()->GetTimerManager().SetTimer(FireCooldownTimer, this, &UShootingComponent::ResetFireCooldown, 1.f / FireRate, false);
}

void UShootingComponent::ResetFireCooldown()
{
	bCanFire = true;
}

