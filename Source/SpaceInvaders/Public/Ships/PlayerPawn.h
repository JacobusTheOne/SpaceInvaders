#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "InputActionValue.h"
#include "Engine/DamageEvents.h"
#include "PlayerPawn.generated.h"

class UInputMappingContext;
class UInputAction;
class UStaticMeshComponent;
class UBoxComponent;
class UShootingComponent;

UCLASS()
class SPACEINVADERS_API APlayerPawn : public APawn
{
    GENERATED_BODY()

public:
    APlayerPawn();

protected:
    virtual void BeginPlay() override;
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
    virtual void Tick(float DeltaTime) override;
    virtual float TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent,
        AController* EventInstigator, AActor* DamageCauser) override;

private:
    // Components
    UPROPERTY(VisibleAnywhere, Category = "Components")
    UBoxComponent* CollisionComponent;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    UStaticMeshComponent* MeshComponent;

    UPROPERTY(VisibleAnywhere, Category = "Components")
    UShootingComponent* ShootingComponent;

    // Input assets
    UPROPERTY(EditDefaultsOnly, Category = "Input")
    UInputMappingContext* MappingContext;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    UInputAction* MoveAction;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    UInputAction* FireAction;

    // Movement
    UPROPERTY(EditDefaultsOnly, Category = "Movement")
    float MoveSpeed = 600.f;

    UPROPERTY(EditDefaultsOnly, Category = "Movement")
    float BoundLeft = -800.f;

    UPROPERTY(EditDefaultsOnly, Category = "Movement")
    float BoundRight = 800.f;

    UPROPERTY(EditDefaultsOnly, Category = "Movement")
    float BoundTop = 400.f;

    UPROPERTY(EditDefaultsOnly, Category = "Movement")
    float BoundBottom = -400.f;

    // Input callbacks
    void OnMove(const FInputActionValue& Value);
    void OnMoveStop();
    void OnFire();
    void OnFireStop();

    UFUNCTION()
    void OnCollisionOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
        bool bFromSweep, const FHitResult& SweepResult);

private:
    FVector2D CurrentMoveInput;
    bool bIsDead = false;
};