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

protected:
    virtual void BeginPlay() override;

private:
    // The projectile class to spawn
    UPROPERTY(EditDefaultsOnly, Category = "Shooting")
    TSubclassOf<AProjectile> ProjectileClass;

    // Rounds per second
    UPROPERTY(EditDefaultsOnly, Category = "Shooting")
    float FireRate = 2.f;

    bool bCanFire = true;
    FTimerHandle FireCooldownTimer;

    void ResetFireCooldown();
};