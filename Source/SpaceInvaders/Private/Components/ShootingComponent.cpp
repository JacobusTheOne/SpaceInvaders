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

	const FVector SpawnLocation = GetComponentLocation();
	const FRotator SpawnRotation = FRotator::ZeroRotator;

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = Owner;
	SpawnParams.Instigator = Owner->GetInstigator();

	World->SpawnActor<AProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, SpawnParams);

	bCanFire = false;
	World->GetTimerManager().SetTimer(FireCooldownTimer, this, &UShootingComponent::ResetFireCooldown, 1.f / FireRate, false);
}

void UShootingComponent::ResetFireCooldown()
{
	bCanFire = true;
}

