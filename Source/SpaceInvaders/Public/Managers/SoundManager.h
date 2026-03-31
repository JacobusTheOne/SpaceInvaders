#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SoundManager.generated.h"

class USoundBase;
class UAudioComponent;

USTRUCT(BlueprintType)
struct FPlayerSounds
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player")
	USoundBase* Shoot = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player")
	USoundBase* Death = nullptr;
};

USTRUCT(BlueprintType)
struct FEnemySounds
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy")
	USoundBase* Shoot = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy")
	USoundBase* Death = nullptr;
};

USTRUCT(BlueprintType)
struct FBossSounds
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss")
	USoundBase* SpreadShot = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss")
	USoundBase* AimedShot = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss")
	USoundBase* ChargeStart = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss")
	USoundBase* ChargeEnd = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss")
	USoundBase* PhaseChange = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss")
	USoundBase* Hit = nullptr;
};

USTRUCT(BlueprintType)
struct FMusicTracks
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Music")
	USoundBase* Gameplay = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Music")
	USoundBase* Boss = nullptr;
};

UCLASS()
class SPACEINVADERS_API ASoundManager : public AActor
{
	GENERATED_BODY()

public:
	ASoundManager();

	// Returns the SoundManager placed in the level, or nullptr if none exists.
	static ASoundManager* Get(UWorld* World);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SFX|Player")
	FPlayerSounds PlayerSounds;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SFX|Enemy")
	FEnemySounds EnemySounds;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SFX|Boss")
	FBossSounds BossSounds;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Music")
	FMusicTracks Music;

	// Duration (seconds) of the crossfade between music tracks
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Music", meta = (ClampMin = "0.0"))
	float MusicFadeDuration = 2.f;

	// Volume multiplier for all SFX [0..1]
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Volume", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float SFXVolume = 1.f;

	// Volume multiplier for music [0..1]
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Volume", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float MusicVolume = 1.f;

	UFUNCTION(BlueprintCallable, Category = "Volume")
	void SetSFXVolume(float NewVolume);

	UFUNCTION(BlueprintCallable, Category = "Volume")
	void SetMusicVolume(float NewVolume);

	UFUNCTION(BlueprintPure, Category = "Volume")
	float GetSFXVolume() const { return SFXVolume; }

	UFUNCTION(BlueprintPure, Category = "Volume")
	float GetMusicVolume() const { return MusicVolume; }

	UFUNCTION(BlueprintCallable, Category = "Music")
	void PlayGameplayMusic();

	UFUNCTION(BlueprintCallable, Category = "Music")
	void PlayBossMusic();

	UFUNCTION(BlueprintCallable, Category = "Music")
	void StopMusic();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, Category = "Music")
	UAudioComponent* GameplayMusicComponent;

	UPROPERTY(VisibleAnywhere, Category = "Music")
	UAudioComponent* BossMusicComponent;
};
