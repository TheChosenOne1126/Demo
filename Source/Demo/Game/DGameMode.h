// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/GameMode.h"
#include "DGameMode.generated.h"

/**
 * 
 */
UCLASS()
class DEMO_API ADGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
	ADGameMode();
	
	virtual APawn* SpawnDefaultPawnFor_Implementation(AController* NewPlayer, AActor* StartSpot) override;

protected:
	virtual void OnPostLogin(AController* NewPlayer) override;

	UPROPERTY(EditDefaultsOnly)
	bool bDefaultSpawnPawn;
};
