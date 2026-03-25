// Fill out your copyright notice in the Description page of Project Settings.


#include "SpaceInvadersPlayerController.h"
#include "Camera/CameraActor.h"
#include "EngineUtils.h"

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
}
