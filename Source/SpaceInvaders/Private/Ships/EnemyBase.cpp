// Fill out your copyright notice in the Description page of Project Settings.


#include "Ships/EnemyBase.h"
#include "Components/EnemySFXComponent.h"
#include "Components/BoxComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "GameState/SpaceInvaderGameState.h"
#include "Components/ShootingComponent.h"
#include "Components/Projectile.h"
#include "Kismet/GameplayStatics.h"
#include "Formation/EnemyFormation.h"

// Sets default values
AEnemyBase::AEnemyBase()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = SceneRoot;

	MeshHull = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshHull"));
	MeshHull->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
	MeshHull->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshHull->SetupAttachment(RootComponent);

	MeshCockpit = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshCockpit"));
	MeshCockpit->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshCockpit->SetupAttachment(MeshHull);

	MeshLeftWing = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshLeftWing"));
	MeshLeftWing->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshLeftWing->SetupAttachment(MeshHull);

	MeshRightWing = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshRightWing"));
	MeshRightWing->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshRightWing->SetupAttachment(MeshHull);

	MeshLeftGun = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshLeftGun"));
	MeshLeftGun->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshLeftGun->SetupAttachment(MeshHull);

	MeshRightGun = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshRightGun"));
	MeshRightGun->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshRightGun->SetupAttachment(MeshHull);

	MeshEngine = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshEngine"));
	MeshEngine->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshEngine->SetupAttachment(MeshHull);

	CollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionComponent"));
	CollisionComponent->SetCollisionProfileName(TEXT("Enemy"));
	CollisionComponent->SetGenerateOverlapEvents(true);
	CollisionComponent->SetupAttachment(RootComponent);

	ShootingComponent = CreateDefaultSubobject<UShootingComponent>(TEXT("ShootingComponent"));
	ShootingComponent->SetupAttachment(RootComponent);
	ShootingComponent->FireDirection = FVector(-1.f, 0.f, 0.f); // fire toward player (-X)

	SFXComponent = CreateDefaultSubobject<UEnemySFXComponent>(TEXT("SFXComponent"));
}

// Called when the game starts or when spawned
void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();

	ScheduleNextShot();
}

void AEnemyBase::InitFormationData(AEnemyFormation* InFormation, int32 InColumnIndex)
{
	Formation    = InFormation;
	ColumnIndex  = InColumnIndex;
}

void AEnemyBase::ScheduleNextShot()
{
	const float Delay = FMath::RandRange(MinFireInterval, MaxFireInterval);
	GetWorldTimerManager().SetTimer(FireIntervalTimer, this, &AEnemyBase::FireAndReschedule, Delay, false);
}

void AEnemyBase::FireAndReschedule()
{
	// Only fire if this enemy is the bottommost alive in its column
	if (Formation.IsValid() && Formation->GetBottommostInColumn(ColumnIndex) != this)
	{
		ScheduleNextShot();
		return;
	}

	UWorld* World = GetWorld();
	TSubclassOf<AProjectile> ProjClass = ShootingComponent->GetProjectileClass();

	if (World && ProjClass)
	{
		const FTransform SpawnTransform(ShootingComponent->FireDirection.Rotation(),
		                                ShootingComponent->GetComponentLocation());

		AProjectile* Proj = World->SpawnActorDeferred<AProjectile>(ProjClass, SpawnTransform, this, GetInstigator());
		if (Proj)
		{
			if (ProjectileInitialSpeed > 0.f) Proj->SetInitialSpeed(ProjectileInitialSpeed);
			if (ProjectileMaxSpeed     > 0.f) Proj->SetMaxSpeed(ProjectileMaxSpeed);
			if (ProjectileLifeSpan     > 0.f) Proj->SetLifeSpanDuration(ProjectileLifeSpan);
			if (ProjectileDamage       > 0.f) Proj->SetDamage(ProjectileDamage);
			Proj->Tags.Add(FName("EnemyProjectile"));
			UGameplayStatics::FinishSpawningActor(Proj, SpawnTransform);
		}

		SFXComponent->PlayShoot();
	}

	ScheduleNextShot();
}

// Called every frame
void AEnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

float AEnemyBase::TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent,
	AController* EventInstigator, AActor* DamageCauser)
{
	if (DamageCauser && DamageCauser->ActorHasTag(FName("EnemyProjectile"))) return 0.f;

	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	Health -= FMath::FloorToInt(ActualDamage);

	if (Health <= 0)
	{
		if (ASpaceInvaderGameState* GS = GetWorld()->GetGameState<ASpaceInvaderGameState>())
		{
			GS->AddScore(ScoreValue);
		}

		SFXComponent->PlayDeath();

		if (ExplosionEffect)
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ExplosionEffect, GetActorLocation());
		}

		UE_LOG(LogTemp, Log, TEXT("EnemyBase: %s destroyed — awarded %d points"), *GetName(), ScoreValue);
		Destroy();
	}

	return ActualDamage;
}


