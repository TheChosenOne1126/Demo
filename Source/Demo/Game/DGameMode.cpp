// Fill out your copyright notice in the Description page of Project Settings.

#include "DGameMode.h"
#include "Player/DPlayerController.h"

ADGameMode::ADGameMode()
{
	bDefaultSpawnPawn = false;
}

APawn* ADGameMode::SpawnDefaultPawnFor_Implementation(AController* NewPlayer, AActor* StartSpot)
{
	if (!bDefaultSpawnPawn)
	{
		return nullptr;
	}
	
	return Super::SpawnDefaultPawnFor_Implementation(NewPlayer, StartSpot);
}

void ADGameMode::OnPostLogin(AController* NewPlayer)
{
	Super::OnPostLogin(NewPlayer);
	
	ADPlayerController* PC = Cast<ADPlayerController>(NewPlayer);
	if (IsValid(PC))
	{
		PC->ClientPostLogin();
	}
}
