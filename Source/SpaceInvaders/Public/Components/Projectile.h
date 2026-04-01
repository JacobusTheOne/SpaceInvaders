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

    // Sets InitialSpeed/MaxSpeed and immediately updates the movement velocity.
    // Call after Activate() to override the pool default for a single shot.
    UFUNCTION(BlueprintCallable, Category = "Projectile")
    void OverrideSpeed(float NewSpeed);

    // Pool support — called by AWaveManager
    void Activate(const FTransform& SpawnTransform, const FVector& FireDirection);
    void ReturnToPool();

    // Reverses this projectile toward enemies and re-tags it as a player projectile
    UFUNCTION(BlueprintCallable, Category = "Projectile")
    void PlayerDeflect(AActor* NewInstigator);

    // Set to true on pool-owned instances before FinishSpawningActor
    UPROPERTY(BlueprintReadWrite, Category = "Projectile")
    bool bIsPooled = false;

protected:
    virtual void BeginPlay() override;

private:
    UPROPERTY(VisibleAnywhere, Category = "Components", meta = (AllowPrivateAcces = "true"))
    USphereComponent* CollisionComponent;

    UPROPERTY(VisibleAnywhere, Category = "Components", meta = (AllowPrivateAcces = "true"))
    UStaticMeshComponent* MeshComponent;

    UPROPERTY(VisibleAnywhere, Category = "Components", meta = (AllowPrivateAcces = "true"))
    UProjectileMovementComponent* ProjectileMovement;

    UFUNCTION()
    void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, FVector NormalImpulse,
        const FHitResult& Hit);

    UPROPERTY(EditDefaultsOnly, Category = "Projectile", meta = (AllowPrivateAcces = "true"))
    float InitialSpeed = 1200.f;

    UPROPERTY(EditDefaultsOnly, Category = "Projectile", meta = (AllowPrivateAcces = "true"))
    float MaxSpeed = 1200.f;

    UPROPERTY(EditDefaultsOnly, Category = "Projectile", meta = (AllowPrivateAcces = "true"))
    float LifeSpan = 3.f;

    UPROPERTY(EditDefaultsOnly, Category = "Projectile", meta = (AllowPrivateAcces = "true"))
    float Damage = 1.0f;

    // Collision profile applied to the sphere at BeginPlay.
    // Set "ProjectilePlayer" on BP_Projectile_Player and "ProjectileEnemy" on BP_Projectile_Enemy.
    UPROPERTY(EditDefaultsOnly, Category = "Projectile", meta = (AllowPrivateAcces = "true"))
    FName CollisionProfileName = FName("Projectile");

    FTimerHandle PoolLifeSpanTimer;
    FTimerHandle DeflectKickTimerHandle;

    bool    bDeflected            = false;
    FVector PendingDeflectVelocity = FVector::ZeroVector;

    void ApplyDeflectVelocity();
};