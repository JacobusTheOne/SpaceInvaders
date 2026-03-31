#include "Components/EnemySFXComponent.h"
#include "Managers/SoundManager.h"

void UEnemySFXComponent::PlayShoot() const
{
	if (!CachedSoundManager) return;
	PlayAtOwner(CachedSoundManager->EnemySounds.Shoot);
}

void UEnemySFXComponent::PlayDeath() const
{
	if (!CachedSoundManager) return;
	PlayAtOwner(CachedSoundManager->EnemySounds.Death);
}
