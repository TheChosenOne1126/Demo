// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "InputAction.h"
#include "Engine/DataAsset.h"
#include "InputDataAsset.generated.h"

class UInputMappingContext;

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
	TMap<FGameplayTag, TObjectPtr<UInputAction>> NativeInputMap;

	UPROPERTY(EditDefaultsOnly)
	TMap<FGameplayTag, TObjectPtr<UInputAction>> AbilityInputMap;

	UPROPERTY(EditDefaultsOnly)
	FInputMappingData InputMappingData;
};