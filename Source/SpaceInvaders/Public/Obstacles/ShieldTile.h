#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ShieldTile.generated.h"

class UBoxComponent;
class UStaticMeshComponent;
class UHealthComponent;

UCLASS()
class SPACEINVADERS_API AShieldTile : public AActor
{
	GENERATED_BODY()

public:
	AShieldTile();

protected:
	virtual void BeginPlay() override;
	virtual float TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent,
	                         AController* EventInstigator, AActor* DamageCauser) override;

private:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UBoxComponent* CollisionComponent;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UHealthComponent* HealthComponent;

	// Assigned in the Blueprint — swapped in when 1 HP remains
	UPROPERTY(EditDefaultsOnly, Category = "Shield", meta = (AllowPrivateAccess = "true"))
	UMaterialInterface* CrackedMaterial;

	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	               UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	               bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnDeath();
};
