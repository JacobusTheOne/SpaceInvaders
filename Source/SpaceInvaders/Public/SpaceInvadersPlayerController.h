// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SpaceInvadersPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class SPACEINVADERS_API ASpaceInvadersPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	ASpaceInvadersPlayerController();

protected:
	virtual void BeginPlay() override;
};
