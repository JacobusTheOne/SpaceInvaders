#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StrafeCooldownWidget.generated.h"

class UProgressBar;

UCLASS()
class SPACEINVADERS_API UStrafeCooldownWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// Called by PlayerPawn every tick — Remaining in [0, Total]
	UFUNCTION(BlueprintCallable, Category = "Strafe")
	void UpdateCooldown(float Remaining, float Total);

protected:
	// Bind in UMG — fill to show cooldown progress (1 = ready, 0 = on cooldown)
	UPROPERTY(meta = (BindWidgetOptional))
	UProgressBar* CooldownBar;
};
