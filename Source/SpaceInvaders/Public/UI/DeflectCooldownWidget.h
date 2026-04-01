#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DeflectCooldownWidget.generated.h"

class UProgressBar;

UCLASS()
class SPACEINVADERS_API UDeflectCooldownWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// Called by PlayerPawn every tick — Remaining in [0, Total]
	UFUNCTION(BlueprintCallable, Category = "Deflect")
	void UpdateCooldown(float Remaining, float Total);

protected:
	virtual void NativeConstruct() override;

	// Bind in UMG — fill to show cooldown progress (1 = ready, 0 = on cooldown)
	UPROPERTY(meta = (BindWidgetOptional))
	UProgressBar* CooldownBar;
};
