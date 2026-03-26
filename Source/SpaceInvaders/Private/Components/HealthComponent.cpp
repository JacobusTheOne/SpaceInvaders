#include "Components/HealthComponent.h"

UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	Health = MaxHealth;

	if (AActor* Owner = GetOwner())
	{
		Owner->OnTakeAnyDamage.AddDynamic(this, &UHealthComponent::HandleTakeDamage);
	}
}

void UHealthComponent::HandleTakeDamage(AActor* DamagedActor, float Damage,
                                         const UDamageType* DamageType,
                                         AController* InstigatedBy, AActor* DamageCauser)
{
	if (Health <= 0) return;

	Health -= FMath::Max(1, FMath::FloorToInt(Damage));

	if (Health <= 0)
	{
		OnDeath.Broadcast();
	}
}
