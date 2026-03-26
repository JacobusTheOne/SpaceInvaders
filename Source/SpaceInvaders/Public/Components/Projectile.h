#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

class UStaticMeshComponent;
class UProjectileMovementComponent;
class USphereComponent;

UCLASS()
class SPACEINVADERS_API AProjectile : public AActor
{
    GENERATED_BODY()

public:
    AProjectile();

    UFUNCTION(BlueprintPure, Category = "Projectile")
    float GetInitialSpeed() const { return InitialSpeed; }

    UFUNCTION(BlueprintPure, Category = "Projectile")
    float GetMaxSpeed() const { return MaxSpeed; }

    // Returns the configured duration, not the remaining lifespan timer
    UFUNCTION(BlueprintPure, Category = "Projectile")
    float GetLifeSpanDuration() const { return LifeSpan; }

    UFUNCTION(BlueprintPure, Category = "Projectile")
    float GetDamage() const { return Damage; }

    UFUNCTION(BlueprintCallable, Category = "Projectile")
    void SetInitialSpeed(float NewSpeed) { InitialSpeed = NewSpeed; }

    UFUNCTION(BlueprintCallable, Category = "Projectile")
    void SetMaxSpeed(float NewMaxSpeed) { MaxSpeed = NewMaxSpeed; }

    // Sets the configured duration used in BeginPlay (does not restart the timer)
    UFUNCTION(BlueprintCallable, Category = "Projectile")
    void SetLifeSpanDuration(float NewDuration) { LifeSpan = NewDuration; }

    UFUNCTION(BlueprintCallable, Category = "Projectile")
    void SetDamage(float NewDamage) { Damage = NewDamage; }

protected:
    virtual void BeginPlay() override;

private:
    UPROPERTY(VisibleAnywhere, Category = "Components")
    USphereComponent* CollisionComponent;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    UStaticMeshComponent* MeshComponent;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    UProjectileMovementComponent* ProjectileMovement;

    UFUNCTION()
    void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, FVector NormalImpulse,
        const FHitResult& Hit);

    UPROPERTY(EditDefaultsOnly, Category = "Projectile")
    float InitialSpeed = 1200.f;

    UPROPERTY(EditDefaultsOnly, Category = "Projectile")
    float MaxSpeed = 1200.f;

    UPROPERTY(EditDefaultsOnly, Category = "Projectile")
    float LifeSpan = 3.f;

    UPROPERTY(EditDefaultsOnly, Category = "Projectile")
    float Damage = 1.0f;



};