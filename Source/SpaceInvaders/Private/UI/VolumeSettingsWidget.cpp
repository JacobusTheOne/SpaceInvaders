#include "UI/VolumeSettingsWidget.h"
#include "Managers/SoundManager.h"
#include "Components/Slider.h"
#include "Components/Button.h"

void UVolumeSettingsWidget::NativeConstruct()
{
	Super::NativeConstruct();

	CachedSoundManager = ASoundManager::Get(GetWorld());

	if (SFXSlider)
	{
		SFXSlider->SetMinValue(0.f);
		SFXSlider->SetMaxValue(1.f);
		SFXSlider->SetValue(CachedSoundManager ? CachedSoundManager->GetSFXVolume() : 1.f);
		SFXSlider->OnValueChanged.AddDynamic(this, &UVolumeSettingsWidget::OnSFXSliderChanged);
	}

	if (MusicSlider)
	{
		MusicSlider->SetMinValue(0.f);
		MusicSlider->SetMaxValue(1.f);
		MusicSlider->SetValue(CachedSoundManager ? CachedSoundManager->GetMusicVolume() : 1.f);
		MusicSlider->OnValueChanged.AddDynamic(this, &UVolumeSettingsWidget::OnMusicSliderChanged);
	}

	if (CloseButton)
	{
		CloseButton->OnClicked.AddDynamic(this, &UVolumeSettingsWidget::OnCloseClicked);
	}
}

void UVolumeSettingsWidget::OnCloseClicked()
{
	RemoveFromParent();
}

void UVolumeSettingsWidget::OnSFXSliderChanged(float Value)
{
	if (CachedSoundManager)
	{
		CachedSoundManager->SetSFXVolume(Value);
	}
}

void UVolumeSettingsWidget::OnMusicSliderChanged(float Value)
{
	if (CachedSoundManager)
	{
		CachedSoundManager->SetMusicVolume(Value);
	}
}
