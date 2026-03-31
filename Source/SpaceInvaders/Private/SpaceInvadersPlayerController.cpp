// Fill out your copyright notice in the Description page of Project Settings.


#include "SpaceInvadersPlayerController.h"
#include "GameModes/SpaceInvaderGameModeBase.h"
#include "Camera/CameraActor.h"
#include "EngineUtils.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

ASpaceInvadersPlayerController::ASpaceInvadersPlayerController()
{
	bAutoManageActiveCameraTarget = false;
}

void ASpaceInvadersPlayerController::BeginPlay()
{
	Super::BeginPlay();

    TActorIterator<ACameraActor> It(GetWorld());
    if (It)
    {
        SetViewTargetWithBlend(*It);
    }

	if (EscapeMappingContext)
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(EscapeMappingContext, 0);
		}
	}
}

void ASpaceInvadersPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(InputComponent))
	{
		if (EscapeAction)
		{
			EIC->BindAction(EscapeAction, ETriggerEvent::Started, this, &ASpaceInvadersPlayerController::OnEscapePressed);
		}
	}
}

void ASpaceInvadersPlayerController::OnEscapePressed()
{
	if (ASpaceInvaderGameModeBase* GM = GetWorld()->GetAuthGameMode<ASpaceInvaderGameModeBase>())
	{
		GM->TogglePause();
	}
}
