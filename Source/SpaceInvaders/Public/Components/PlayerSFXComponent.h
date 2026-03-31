#pragma once

#include "CoreMinimal.h"
#include "Components/SFXComponent.h"
#include "PlayerSFXComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SPACEINVADERS_API UPlayerSFXComponent : public USFXComponent
{
	GENERATED_BODY()

public:
	void PlayShoot() const;
	void PlayDeath() const;
};
