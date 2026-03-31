// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "SpaceInvadersPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;

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
	virtual void SetupInputComponent() override;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputMappingContext* EscapeMappingContext;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UInputAction* EscapeAction;

private:
	void OnEscapePressed();
};
