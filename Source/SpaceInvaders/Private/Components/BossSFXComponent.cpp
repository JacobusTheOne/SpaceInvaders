#include "Components/BossSFXComponent.h"
#include "Managers/SoundManager.h"

void UBossSFXComponent::PlaySpreadShot() const
{
	if (!CachedSoundManager) return;
	PlayAtOwner(CachedSoundManager->BossSounds.SpreadShot);
}

void UBossSFXComponent::PlayAimedShot() const
{
	if (!CachedSoundManager) return;
	PlayAtOwner(CachedSoundManager->BossSounds.AimedShot);
}

void UBossSFXComponent::PlayChargeStart() const
{
	if (!CachedSoundManager) return;
	PlayAtOwner(CachedSoundManager->BossSounds.ChargeStart);
}

void UBossSFXComponent::PlayChargeEnd() const
{
	if (!CachedSoundManager) return;
	PlayAtOwner(CachedSoundManager->BossSounds.ChargeEnd);
}

void UBossSFXComponent::PlayPhaseChange() const
{
	if (!CachedSoundManager) return;
	PlayAtOwner(CachedSoundManager->BossSounds.PhaseChange);
}

void UBossSFXComponent::PlayHit() const
{
	if (!CachedSoundManager) return;
	PlayAtOwner(CachedSoundManager->BossSounds.Hit);
}
