#include "Components/PlayerSFXComponent.h"
#include "Managers/SoundManager.h"

void UPlayerSFXComponent::PlayShoot() const
{
	if (!CachedSoundManager) return;
	PlayAtOwner(CachedSoundManager->PlayerSounds.Shoot);
}

void UPlayerSFXComponent::PlayDeath() const
{
	if (!CachedSoundManager) return;
	PlayAtOwner(CachedSoundManager->PlayerSounds.Death);
}
