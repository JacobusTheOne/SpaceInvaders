#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeathSignature);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SPACEINVADERS_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UHealthComponent();

	UPROPERTY(BlueprintAssignable, Category = "Health")
	FOnDeathSignature OnDeath;

	UFUNCTION(BlueprintPure, Category = "Health")
	int32 GetHealth() const { return Health; }

	UFUNCTION(BlueprintPure, Category = "Health")
	bool IsDead() const { return Health <= 0; }

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Health")
	int32 MaxHealth = 2;

	int32 Health = 0;

	UFUNCTION()
	void HandleTakeDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
	                      AController* InstigatedBy, AActor* DamageCauser);
};
