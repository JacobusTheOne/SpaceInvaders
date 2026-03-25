// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FormationMovementComponent.generated.h"

class AEnemyBase;
class AEnemyFormation;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SPACEINVADERS_API UFormationMovementComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UFormationMovementComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	void Step();
	void DropAndReverse();

	AEnemyBase* RightmostAlive() const;
	AEnemyBase* LeftmostAlive() const;

	UPROPERTY(EditAnywhere, Category = "Formation Movement")
	float MoveStep = 40.f;

	UPROPERTY(EditAnywhere, Category = "Formation Movement")
	float StepInterval = 0.8f;

	UPROPERTY(EditAnywhere, Category = "Formation Movement")
	float DropAmount = 80.f;

	UPROPERTY(EditAnywhere, Category = "Formation Movement")
	float RightBound = 560.f;

	UPROPERTY(EditAnywhere, Category = "Formation Movement")
	float LeftBound = -560.f;

	// 1 = right (+Y), -1 = left (-Y)
	int32 Direction = 1;

	float StepAccumulator = 0.f;

	UPROPERTY()
	AEnemyFormation* Formation = nullptr;
};
