#include "UI/WaveClearedUI.h"
#include "Components/TextBlock.h"

void UWaveClearedUI::StartSequence()
{
	CountdownValue = 3;

	if (HeaderText)
	{
		HeaderText->SetText(FText::FromString(TEXT("Wave Cleared!")));
	}
	if (CountdownText)
	{
		CountdownText->SetText(FText::FromString(TEXT("Get Ready!")));
	}

	// Pause on "Get Ready!" for 1.5 s, then start the numeric countdown
	GetWorld()->GetTimerManager().SetTimer(
		CountdownTimer, this, &UWaveClearedUI::TickCountdown, 1.5f, false);
}

void UWaveClearedUI::StartGameOverSequence()
{
	bIsGameOver = true;
	CountdownValue = 3;

	if (HeaderText)
	{
		HeaderText->SetText(FText::FromString(TEXT("Game Over!")));
	}
	if (CountdownText)
	{
		CountdownText->SetText(FText::FromString(TEXT("Restarting...")));
	}

	GetWorld()->GetTimerManager().SetTimer(
		CountdownTimer, this, &UWaveClearedUI::TickCountdown, 1.5f, false);
}

void UWaveClearedUI::TickCountdown()
{
	if (CountdownValue > 0)
	{
		const FString Label = bIsGameOver
			? FString::Printf(TEXT("Restarting in %d"), CountdownValue)
			: FString::FromInt(CountdownValue);

		if (CountdownText)
		{
			CountdownText->SetText(FText::FromString(Label));
		}
		--CountdownValue;

		GetWorld()->GetTimerManager().SetTimer(
			CountdownTimer, this, &UWaveClearedUI::TickCountdown, 1.f, false);
	}
	else
	{
		RemoveFromParent();
		OnCountdownFinished.Broadcast();
	}
}
