#include "Components/Projectile.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

AProjectile::AProjectile()
{
    PrimaryActorTick.bCanEverTick = false;

    // Collision is the root � sphere keeps it simple for now
    CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
    CollisionComponent->SetSphereRadius(10.f);
    RootComponent = CollisionComponent;

    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
    MeshComponent->SetupAttachment(RootComponent);
    MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
    ProjectileMovement->bRotationFollowsVelocity = false;
    ProjectileMovement->bShouldBounce = false;
    ProjectileMovement->ProjectileGravityScale = 0.f;
}

void AProjectile::BeginPlay()
{
    Super::BeginPlay();

    CollisionComponent->SetCollisionProfileName(CollisionProfileName);
    CollisionComponent->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);

    // Pooled instances stay hidden and inert — Activate() configures them at use-time
    if (bIsPooled) return;

    ProjectileMovement->InitialSpeed = InitialSpeed;
    ProjectileMovement->MaxSpeed     = MaxSpeed;
    ProjectileMovement->Velocity     = GetActorForwardVector() * InitialSpeed;
    SetLifeSpan(LifeSpan);
}

void AProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, FVector NormalImpulse,
    const FHitResult& Hit)
{
    if (!OtherActor || OtherActor == GetOwner()) return;

    UGameplayStatics::ApplyDamage(
        OtherActor,
        Damage,
        GetInstigatorController(),
        this,
        UDamageType::StaticClass()
    );

    if (bIsPooled) ReturnToPool();
    else           Destroy();
}

void AProjectile::Activate(const FTransform& SpawnTransform, const FVector& FireDirection)
{
    SetActorTransform(SpawnTransform);
    SetActorHiddenInGame(false);
    CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    ProjectileMovement->SetComponentTickEnabled(true);
    ProjectileMovement->MaxSpeed = MaxSpeed;
    ProjectileMovement->Velocity = FireDirection.GetSafeNormal() * InitialSpeed;

    GetWorldTimerManager().SetTimer(
        PoolLifeSpanTimer, this, &AProjectile::ReturnToPool, LifeSpan, false);
}

void AProjectile::OverrideSpeed(float NewSpeed)
{
    InitialSpeed = NewSpeed;
    MaxSpeed     = NewSpeed;
    ProjectileMovement->MaxSpeed = NewSpeed;
    ProjectileMovement->Velocity = ProjectileMovement->Velocity.GetSafeNormal() * NewSpeed;
}

void AProjectile::ReturnToPool()
{
    GetWorldTimerManager().ClearTimer(PoolLifeSpanTimer);
    ProjectileMovement->StopMovementImmediately();
    ProjectileMovement->SetComponentTickEnabled(false);
    CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    SetActorHiddenInGame(true);
}