#include "Managers/SoundManager.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"

ASoundManager::ASoundManager()
{
	PrimaryActorTick.bCanEverTick = false;

	GameplayMusicComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("GameplayMusicComponent"));
	GameplayMusicComponent->bAutoActivate = false;
	GameplayMusicComponent->SetUISound(true);

	BossMusicComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("BossMusicComponent"));
	BossMusicComponent->bAutoActivate = false;
	BossMusicComponent->SetUISound(true);
}

ASoundManager* ASoundManager::Get(UWorld* World)
{
	if (!World) return nullptr;
	return Cast<ASoundManager>(
		UGameplayStatics::GetActorOfClass(World, ASoundManager::StaticClass()));
}

void ASoundManager::BeginPlay()
{
	Super::BeginPlay();

	if (Music.Gameplay)
	{
		GameplayMusicComponent->SetSound(Music.Gameplay);
	}
	if (Music.Boss)
	{
		BossMusicComponent->SetSound(Music.Boss);
	}

	PlayGameplayMusic();
}

void ASoundManager::SetSFXVolume(float NewVolume)
{
	SFXVolume = FMath::Clamp(NewVolume, 0.f, 1.f);
}

void ASoundManager::SetMusicVolume(float NewVolume)
{
	MusicVolume = FMath::Clamp(NewVolume, 0.f, 1.f);
	GameplayMusicComponent->SetVolumeMultiplier(MusicVolume);
	BossMusicComponent->SetVolumeMultiplier(MusicVolume);
}

void ASoundManager::PlayGameplayMusic()
{
	if (!Music.Gameplay) return;

	BossMusicComponent->FadeOut(MusicFadeDuration, 0.f);
	GameplayMusicComponent->FadeIn(MusicFadeDuration, MusicVolume);
}

void ASoundManager::PlayBossMusic()
{
	if (!Music.Boss) return;

	GameplayMusicComponent->FadeOut(MusicFadeDuration, 0.f);
	BossMusicComponent->FadeIn(MusicFadeDuration, MusicVolume);
}

void ASoundManager::StopMusic()
{
	GameplayMusicComponent->FadeOut(MusicFadeDuration, 0.f);
	BossMusicComponent->FadeOut(MusicFadeDuration, 0.f);
}
