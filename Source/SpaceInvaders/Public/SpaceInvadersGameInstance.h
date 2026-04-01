#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "SpaceInvadersGameInstance.generated.h"

// Persists for the entire game session — survives level loads.
// Stores audio settings so they carry from MainMenu into gameplay.
UCLASS()
class SPACEINVADERS_API USpaceInvadersGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, Category = "Audio")
	float SFXVolume = 1.f;

	UPROPERTY(BlueprintReadWrite, Category = "Audio")
	float MusicVolume = 1.f;
};
