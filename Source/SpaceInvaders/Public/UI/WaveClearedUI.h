#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WaveClearedUI.generated.h"

class UTextBlock;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCountdownFinishedSignature);

UCLASS()
class SPACEINVADERS_API UWaveClearedUI : public UUserWidget
{
	GENERATED_BODY()

public:
	// Call this to begin the "Wave Cleared! → Get Ready → 3 → 2 → 1" sequence
	void StartSequence();

	// Call this to begin the "Game Over! → Restarting in 3 → 2 → 1" sequence
	void StartGameOverSequence();

	// Broadcast when the countdown reaches zero — WaveManager binds here to start the next wave
	UPROPERTY(BlueprintAssignable, Category = "Wave")
	FOnCountdownFinishedSignature OnCountdownFinished;

private:
	// Advances the countdown each second; fires OnCountdownFinished when done
	void TickCountdown();

	// Names must match the widget names in your WBP Blueprint
	UPROPERTY(meta = (BindWidget))
	UTextBlock* HeaderText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CountdownText;

	FTimerHandle CountdownTimer;
	int32 CountdownValue = 3;
	bool bIsGameOver = false;
};
