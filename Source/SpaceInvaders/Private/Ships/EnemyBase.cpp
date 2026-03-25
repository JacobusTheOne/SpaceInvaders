// Fill out your copyright notice in the Description page of Project Settings.


#include "Ships/EnemyBase.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "GameState/SpaceInvaderGameState.h"

// Sets default values
AEnemyBase::AEnemyBase()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = SceneRoot;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshComponent->SetupAttachment(RootComponent);

	CollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionComponent"));
	CollisionComponent->SetCollisionProfileName(TEXT("Enemy"));
	CollisionComponent->SetGenerateOverlapEvents(true);
	CollisionComponent->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

float AEnemyBase::TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent,
	AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	Health -= FMath::FloorToInt(ActualDamage);

	if (Health <= 0)
	{
		if (ASpaceInvaderGameState* GS = GetWorld()->GetGameState<ASpaceInvaderGameState>())
		{
			GS->AddScore(ScoreValue);
		}

		UE_LOG(LogTemp, Log, TEXT("EnemyBase: %s destroyed — awarded %d points"), *GetName(), ScoreValue);
		Destroy();
	}

	return ActualDamage;
}


