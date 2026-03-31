#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PowerUp.generated.h"

class UBoxComponent;
class UStaticMeshComponent;
class UNiagaraSystem;

UCLASS()
class SPACEINVADERS_API APowerUp : public AActor
{
	GENERATED_BODY()

public:
	APowerUp();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UBoxComponent* CollisionBox;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* MeshComponent;

	// Speed at which the power-up drifts toward the player (-X direction)
	UPROPERTY(EditDefaultsOnly, Category = "PowerUp", meta = (ClampMin = "0.0"))
	float MoveSpeed = 300.f;

	// How much fire rate (rounds/second) is permanently added to the player on pickup
	UPROPERTY(EditDefaultsOnly, Category = "PowerUp", meta = (ClampMin = "0.0"))
	float FireRateBoost = 1.f;

	// Seconds before the power-up auto-destroys if not collected
	UPROPERTY(EditDefaultsOnly, Category = "PowerUp", meta = (ClampMin = "1.0"))
	float LifeSpanDuration = 12.f;

	// Degrees per second the mesh spins around its local X axis
	UPROPERTY(EditDefaultsOnly, Category = "PowerUp|Rotation")
	float XSpinRate = 180.f;

	// Degrees per second the mesh rotates around its local Z axis (slow tumble)
	UPROPERTY(EditDefaultsOnly, Category = "PowerUp|Rotation")
	float ZSpinRate = 45.f;

	UPROPERTY(EditDefaultsOnly, Category = "PowerUp|VFX")
	UNiagaraSystem* PickupEffect;

	UPROPERTY(EditDefaultsOnly, Category = "PowerUp|SFX")
	USoundBase* PickupSound;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);
};
