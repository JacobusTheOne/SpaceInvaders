// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/FormationMovementComponent.h"
#include "Formation/EnemyFormation.h"
#include "Ships/EnemyBase.h"

UFormationMovementComponent::UFormationMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UFormationMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	Formation = Cast<AEnemyFormation>(GetOwner());
	if (!Formation)
	{
		UE_LOG(LogTemp, Warning, TEXT("FormationMovementComponent: owner is not AEnemyFormation"));
	}
}

void UFormationMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!Formation) return;

	StepAccumulator += DeltaTime;
	if (StepAccumulator >= StepInterval)
	{
		StepAccumulator = 0.f;
		Step();
	}
}

void UFormationMovementComponent::Step()
{
	// Check edge before moving
	if (Direction == 1)
	{
		AEnemyBase* Rightmost = RightmostAlive();
		if (Rightmost && Rightmost->GetActorLocation().Y + MoveStep >= RightBound)
		{
			DropAndReverse();
			return;
		}
	}
	else
	{
		AEnemyBase* Leftmost = LeftmostAlive();
		if (Leftmost && Leftmost->GetActorLocation().Y - MoveStep <= LeftBound)
		{
			DropAndReverse();
			return;
		}
	}

	// Move all alive enemies laterally
	const FVector Delta = FVector(0.f, MoveStep * Direction, 0.f);
	for (AEnemyBase* Enemy : Formation->GetGrid())
	{
		if (IsValid(Enemy))
		{
			Enemy->SetActorLocation(Enemy->GetActorLocation() + Delta);
		}
	}
}

void UFormationMovementComponent::DropAndReverse()
{
	Direction *= -1;

	const FVector DropDelta = FVector(-DropAmount, 0.f, 0.f);
	for (AEnemyBase* Enemy : Formation->GetGrid())
	{
		if (IsValid(Enemy))
		{
			Enemy->SetActorLocation(Enemy->GetActorLocation() + DropDelta);
		}
	}

	CheckPlayerThreshold();
}

void UFormationMovementComponent::CheckPlayerThreshold()
{
	if (bGameOverTriggered || !Formation) return;

	for (AEnemyBase* Enemy : Formation->GetGrid())
	{
		if (IsValid(Enemy) && Enemy->GetActorLocation().X <= PlayerXThreshold)
		{
			bGameOverTriggered = true;
			PrimaryComponentTick.bCanEverTick = false;
			Formation->NotifyGameOver();
			return;
		}
	}
}

AEnemyBase* UFormationMovementComponent::RightmostAlive() const
{
	AEnemyBase* Result = nullptr;
	float MaxY = -FLT_MAX;

	for (AEnemyBase* Enemy : Formation->GetGrid())
	{
		if (IsValid(Enemy))
		{
			float Y = Enemy->GetActorLocation().Y;
			if (Y > MaxY)
			{
				MaxY = Y;
				Result = Enemy;
			}
		}
	}

	return Result;
}

AEnemyBase* UFormationMovementComponent::LeftmostAlive() const
{
	AEnemyBase* Result = nullptr;
	float MinY = FLT_MAX;

	for (AEnemyBase* Enemy : Formation->GetGrid())
	{
		if (IsValid(Enemy))
		{
			float Y = Enemy->GetActorLocation().Y;
			if (Y < MinY)
			{
				MinY = Y;
				Result = Enemy;
			}
		}
	}

	return Result;
}
