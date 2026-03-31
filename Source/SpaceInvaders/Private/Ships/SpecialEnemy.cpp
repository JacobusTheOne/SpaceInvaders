#include "Ships/SpecialEnemy.h"
#include "Items/PowerUp.h"

ASpecialEnemy::ASpecialEnemy()
{
	SetHealth(1);
	SetScoreValue(500);
}

float ASpecialEnemy::TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent,
	AController* EventInstigator, AActor* DamageCauser)
{
	// Cache location before Super — which may call Destroy()
	const FVector SpawnLocation = GetActorLocation();

	float Applied = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (Applied > 0.f && PowerUpClass && IsActorBeingDestroyed())
	{
		GetWorld()->SpawnActor<APowerUp>(PowerUpClass, SpawnLocation, FRotator::ZeroRotator);
	}

	return Applied;
}
