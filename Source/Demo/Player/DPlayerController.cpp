// Fill out your copyright notice in the Description page of Project Settings.

#include "DPlayerController.h"

void ADPlayerController::PostProcessInput(const float DeltaTime, const bool bGamePaused)
{
	Super::PostProcessInput(DeltaTime, bGamePaused);
	
	if (bGamePaused)
	{
		return;
	}

	OnPostProcessInput();
}

void ADPlayerController::ClientRestart_Implementation(APawn* NewPawn)
{
	Super::ClientRestart_Implementation(NewPawn);

	if (!IsValid(NewPawn))
	{
		return;
	}
	
	OnClientRestart(NewPawn);
}

FString ADPlayerController::GetModuleName_Implementation() const
{
	return TEXT("Player.PlayerController");
}
