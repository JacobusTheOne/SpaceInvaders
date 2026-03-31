#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SFXComponent.generated.h"

class ASoundManager;
class USoundBase;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SPACEINVADERS_API USFXComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USFXComponent();

protected:
	virtual void BeginPlay() override;

	// Plays Sound at the owning actor's world location. No-op if Sound is null.
	void PlayAtOwner(USoundBase* Sound) const;

	UPROPERTY()
	ASoundManager* CachedSoundManager = nullptr;
};
