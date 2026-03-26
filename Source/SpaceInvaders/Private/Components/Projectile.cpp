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
    CollisionComponent->SetCollisionProfileName(TEXT("Projectile"));
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

    // Apply speed here so Blueprint CDO overrides (EditDefaultsOnly) are respected
    ProjectileMovement->InitialSpeed = InitialSpeed;
    ProjectileMovement->MaxSpeed     = MaxSpeed;

    // Auto destroy after LifeSpan seconds if it hits nothing
    SetLifeSpan(LifeSpan);

    // Bind hit event
    CollisionComponent->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);

    // Fire along the actor's forward vector so spawn rotation controls direction
    ProjectileMovement->Velocity = GetActorForwardVector() * InitialSpeed;
}

void AProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, FVector NormalImpulse,
    const FHitResult& Hit)
{
    if (!OtherActor || OtherActor == GetOwner()) return;

    // Apply damage to whatever was hit
    UGameplayStatics::ApplyDamage(
        OtherActor,
        Damage,
        GetInstigatorController(),
        this,
        UDamageType::StaticClass()
    );

    Destroy();
}