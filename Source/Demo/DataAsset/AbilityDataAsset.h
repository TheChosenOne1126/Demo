// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilitySpecHandle.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "AbilityDataAsset.generated.h"

class UGameplayEffect;
class UGameplayAbility;

USTRUCT(BlueprintType)
struct FAbilityData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, meta = (AllowAbstract = "false"))
	TSubclassOf<UGameplayAbility> AbilityClass;

	UPROPERTY(EditDefaultsOnly)
	int32 InitialAbilityLevel = 0;

	UPROPERTY(EditDefaultsOnly)
	int32 MaxAbilityLevel = 0;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTagContainer Tags;
};

UCLASS()
class DEMO_API UAbilityDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	TArray<FAbilityData> AbilityDataArr;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> InitialEffectClass;
};
