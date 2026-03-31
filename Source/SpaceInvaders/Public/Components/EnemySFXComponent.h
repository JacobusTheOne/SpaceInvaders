#pragma once

#include "CoreMinimal.h"
#include "Components/SFXComponent.h"
#include "EnemySFXComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SPACEINVADERS_API UEnemySFXComponent : public USFXComponent
{
	GENERATED_BODY()

public:
	void PlayShoot() const;
	void PlayDeath() const;
};
