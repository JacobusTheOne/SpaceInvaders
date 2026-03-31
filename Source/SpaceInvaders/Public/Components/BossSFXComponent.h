#pragma once

#include "CoreMinimal.h"
#include "Components/EnemySFXComponent.h"
#include "BossSFXComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SPACEINVADERS_API UBossSFXComponent : public UEnemySFXComponent
{
	GENERATED_BODY()

public:
	void PlaySpreadShot()  const;
	void PlayAimedShot()   const;
	void PlayChargeStart() const;
	void PlayChargeEnd()   const;
	void PlayPhaseChange() const;
	void PlayHit()         const;
};
