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
	virtual APawn* SpawnDefaultPawnAtTransform_Implementation(AController* NewPlayer, const FTransform& SpawnTransform) override;
	
protected:
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag DefaultPawnTag;
};
