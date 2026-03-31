#include "Components/SFXComponent.h"
#include "Managers/SoundManager.h"
#include "Kismet/GameplayStatics.h"

USFXComponent::USFXComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USFXComponent::BeginPlay()
{
	Super::BeginPlay();

	CachedSoundManager = ASoundManager::Get(GetWorld());
	if (!CachedSoundManager)
	{
		UE_LOG(LogTemp, Warning, TEXT("SFXComponent on '%s': no SoundManager found in level — audio will be silent."),
			*GetOwner()->GetName());
	}
}

void USFXComponent::PlayAtOwner(USoundBase* Sound) const
{
	if (!Sound) return;
	AActor* Owner = GetOwner();
	if (!Owner) return;
	const float Volume = CachedSoundManager ? CachedSoundManager->GetSFXVolume() : 1.f;
	UGameplayStatics::PlaySoundAtLocation(Owner, Sound, Owner->GetActorLocation(), Volume);
}
