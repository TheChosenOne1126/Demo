// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "InputAction.h"
#include "Engine/DataAsset.h"
#include "InputDataAsset.generated.h"

class UInputMappingContext;

USTRUCT(BlueprintType)
struct FInputData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputAction> Action;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag Tag;
};

USTRUCT(BlueprintType)
struct FInputMappingData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UInputMappingContext> MappingContext;

	UPROPERTY(EditDefaultsOnly)
	int32 Priority = 0;
};

UCLASS()
class DEMO_API UInputDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	TArray<FInputData> NativeInputs;

	UPROPERTY(EditDefaultsOnly)
	TArray<FInputData> AbilityInputs;

	UPROPERTY(EditDefaultsOnly)
	FInputMappingData InputMappingData;
};