#include "Obstacles/ShieldTile.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/HealthComponent.h"
#include "Ships/EnemyBase.h"
#include "Kismet/GameplayStatics.h"

AShieldTile::AShieldTile()
{
	PrimaryActorTick.bCanEverTick = false;

	CollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionComponent"));
	CollisionComponent->SetCollisionProfileName(TEXT("BlockAll"));
	CollisionComponent->SetGenerateOverlapEvents(true);
	RootComponent = CollisionComponent;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshComponent->SetupAttachment(RootComponent);

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
}

void AShieldTile::BeginPlay()
{
	Super::BeginPlay();

	HealthComponent->OnDeath.AddDynamic(this, &AShieldTile::OnDeath);
	CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AShieldTile::OnOverlap);
}

float AShieldTile::TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent,
                               AController* EventInstigator, AActor* DamageCauser)
{
	const float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (CrackedMaterial && HealthComponent->GetHealth() == 1)
	{
		MeshComponent->SetMaterial(0, CrackedMaterial);
	}

	return ActualDamage;
}

void AShieldTile::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                             UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
                             bool bFromSweep, const FHitResult& SweepResult)
{
	if (AEnemyBase* Enemy = Cast<AEnemyBase>(OtherActor))
	{
		Enemy->Destroy();
		UGameplayStatics::ApplyDamage(this, 1.f, nullptr, Enemy, UDamageType::StaticClass());
	}
}

void AShieldTile::OnDeath()
{
	Destroy();
}
