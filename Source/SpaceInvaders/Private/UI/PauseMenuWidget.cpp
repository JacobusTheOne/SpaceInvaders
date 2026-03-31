#include "UI/PauseMenuWidget.h"
#include "GameModes/SpaceInvaderGameModeBase.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

void UPauseMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ResumeButton)
	{
		ResumeButton->OnClicked.AddDynamic(this, &UPauseMenuWidget::OnResumeClicked);
	}
}

void UPauseMenuWidget::OnResumeClicked()
{
	if (ASpaceInvaderGameModeBase* GM = GetWorld()->GetAuthGameMode<ASpaceInvaderGameModeBase>())
	{
		GM->TogglePause();
	}
}

void UPauseMenuWidget::CloseMenu()
{
	UGameplayStatics::SetGamePaused(this, false);

	if (APlayerController* PC = GetOwningPlayer())
	{
		PC->bShowMouseCursor = false;
		PC->SetInputMode(FInputModeGameOnly());
	}

	RemoveFromParent();
}
