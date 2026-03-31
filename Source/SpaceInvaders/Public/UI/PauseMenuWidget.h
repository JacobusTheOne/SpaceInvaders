#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PauseMenuWidget.generated.h"

class UButton;

UCLASS()
class SPACEINVADERS_API UPauseMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// Called by GameMode to close the menu, unpause, and restore input
	void CloseMenu();

protected:
	virtual void NativeConstruct() override;

private:
	// Name must match the widget name in your WBP Blueprint
	UPROPERTY(meta = (BindWidget))
	UButton* ResumeButton;

	UFUNCTION()
	void OnResumeClicked();
};
