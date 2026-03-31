#pragma once

#include "CoreMinimal.h"
#include "Ships/EnemyBase.h"
#include "SpecialEnemy.generated.h"

class APowerUp;

UCLASS()
class SPACEINVADERS_API ASpecialEnemy : public AEnemyBase
{
	GENERATED_BODY()

public:
	ASpecialEnemy();

	virtual float TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent,
		AController* EventInstigator, AActor* DamageCauser) override;

private:
	// Power-up Blueprint class to spawn when this enemy is destroyed
	UPROPERTY(EditDefaultsOnly, Category = "SpecialEnemy")
	TSubclassOf<APowerUp> PowerUpClass;
};
