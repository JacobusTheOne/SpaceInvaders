// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "EnemyBase.generated.h"

class UBoxComponent;
class USceneComponent;

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

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual float TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent,
		AController* EventInstigator, AActor* DamageCauser) override;

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
private:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	USceneComponent* SceneRoot;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UBoxComponent* CollisionComponent;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* MeshComponent;


};
