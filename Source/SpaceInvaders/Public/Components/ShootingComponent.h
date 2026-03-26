#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "ShootingComponent.generated.h"

class AProjectile;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SPACEINVADERS_API UShootingComponent : public USceneComponent
{
    GENERATED_BODY()

public:
    UShootingComponent();

    void TryFire();

    TSubclassOf<AProjectile> GetProjectileClass() const { return ProjectileClass; }
    bool CanFire() const { return bCanFire; }
    void StartCooldown();

    // Direction projectiles travel (world space, normalised)
    UPROPERTY(EditDefaultsOnly, Category = "Shooting")
    FVector FireDirection = FVector::ForwardVector;

    // Rounds per second
    UPROPERTY(EditDefaultsOnly, Category = "Shooting")
    float FireRate = 2.f;

protected:
    virtual void BeginPlay() override;

private:
    // The projectile class to spawn
    UPROPERTY(EditDefaultsOnly, Category = "Shooting")
    TSubclassOf<AProjectile> ProjectileClass;

    bool bCanFire = true;
    FTimerHandle FireCooldownTimer;

    void ResetFireCooldown();
};