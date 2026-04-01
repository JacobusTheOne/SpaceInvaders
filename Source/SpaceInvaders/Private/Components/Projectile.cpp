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
    UE_LOG(LogTemp, Warning, TEXT("[Projectile] OnHit — OtherActor=%s bDeflected=%d Tags=%d"),
        OtherActor ? *OtherActor->GetName() : TEXT("null"),
        bDeflected,
        Tags.Num());

    if (!OtherActor || OtherActor == GetOwner()) return;

    UGameplayStatics::ApplyDamage(
        OtherActor,
        Damage,
        GetInstigatorController(),
        this,
        UDamageType::StaticClass()
    );

    UE_LOG(LogTemp, Warning, TEXT("[Projectile] Post-ApplyDamage — bDeflected=%d"), bDeflected);

    if (bDeflected) return;

    if (bIsPooled) ReturnToPool();
    else           Destroy();
}

void AProjectile::PlayerDeflect(AActor* NewInstigator)
{
    bDeflected = true;

    // Store the reversed velocity — we cannot apply it now because
    // ProjectileMovementComponent::HandleBlockingHit will call StopSimulating()
    // after OnHit returns (bShouldBounce == false), zeroing the velocity.
    // ApplyDeflectVelocity() re-kicks movement on the next tick instead.
    PendingDeflectVelocity = FVector(
        -ProjectileMovement->Velocity.X,
         ProjectileMovement->Velocity.Y,
         ProjectileMovement->Velocity.Z);

    UE_LOG(LogTemp, Warning, TEXT("[Deflect] PlayerDeflect called — CurrentVel=(%.1f,%.1f,%.1f) PendingDeflect=(%.1f,%.1f,%.1f)"),
        ProjectileMovement->Velocity.X, ProjectileMovement->Velocity.Y, ProjectileMovement->Velocity.Z,
        PendingDeflectVelocity.X, PendingDeflectVelocity.Y, PendingDeflectVelocity.Z);

    CollisionComponent->SetCollisionProfileName(FName("ProjectilePlayer"));
    SetOwner(NewInstigator);
    Tags.Remove(FName("EnemyProjectile"));
    Tags.Add(FName("PlayerProjectile"));

    UE_LOG(LogTemp, Warning, TEXT("[Deflect] Scheduling ApplyDeflectVelocity for next tick"));
    FTimerDelegate Del;
    Del.BindUObject(this, &AProjectile::ApplyDeflectVelocity);
    GetWorldTimerManager().SetTimer(DeflectKickTimerHandle, Del, 0.0001f, false);
}

void AProjectile::ApplyDeflectVelocity()
{
    if (!IsValid(this)) return;

    // StopSimulating() calls SetUpdatedComponent(nullptr), detaching the movement
    // component from the actor so velocity changes have no effect. Re-attach first.
    ProjectileMovement->SetUpdatedComponent(RootComponent);
    ProjectileMovement->SetComponentTickEnabled(true);
    ProjectileMovement->Velocity = PendingDeflectVelocity;

    UE_LOG(LogTemp, Warning, TEXT("[Deflect] ApplyDeflectVelocity — set velocity=(%.1f,%.1f,%.1f) UpdatedComponent=%s"),
        PendingDeflectVelocity.X, PendingDeflectVelocity.Y, PendingDeflectVelocity.Z,
        ProjectileMovement->UpdatedComponent ? *ProjectileMovement->UpdatedComponent->GetName() : TEXT("null"));
}

void AProjectile::Activate(const FTransform& SpawnTransform, const FVector& FireDirection)
{
    GetWorldTimerManager().ClearTimer(DeflectKickTimerHandle);
    bDeflected = false;
    PendingDeflectVelocity = FVector::ZeroVector;
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
    GetWorldTimerManager().ClearTimer(DeflectKickTimerHandle);
    GetWorldTimerManager().ClearTimer(PoolLifeSpanTimer);
    ProjectileMovement->StopMovementImmediately();
    ProjectileMovement->SetComponentTickEnabled(false);
    CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    SetActorHiddenInGame(true);
}