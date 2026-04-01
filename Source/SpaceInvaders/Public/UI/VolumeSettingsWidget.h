#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "VolumeSettingsWidget.generated.h"

class USlider;
class UButton;
class ASoundManager;

DECLARE_MULTICAST_DELEGATE(FOnSettingsClosed);

UCLASS()
class SPACEINVADERS_API UVolumeSettingsWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** Broadcast when the close button is clicked, before collapsing self. */
	FOnSettingsClosed OnClosed;

protected:
	virtual void NativeConstruct() override;

private:
	// Names must match the widget names in your WBP Blueprint
	UPROPERTY(meta = (BindWidget))
	USlider* SFXSlider;

	UPROPERTY(meta = (BindWidget))
	USlider* MusicSlider;

	// Optional — only needed when shown standalone (main menu). Not required when embedded inside PauseMenuWidget.
	UPROPERTY(meta = (BindWidgetOptional))
	UButton* CloseButton;

	UFUNCTION()
	void OnSFXSliderChanged(float Value);

	UFUNCTION()
	void OnMusicSliderChanged(float Value);

	UFUNCTION()
	void OnCloseClicked();

	ASoundManager* CachedSoundManager = nullptr;
};
