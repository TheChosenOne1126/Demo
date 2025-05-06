// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilitySpecHandle.h"
#include "DataAsset/AbilityDataAsset.h"
#include "UObject/Interface.h"
#include "AbilitySlotInterface.generated.h"

USTRUCT()
struct FAbilitySlotData
{
	GENERATED_BODY()

	UPROPERTY()
	FGameplayAbilitySpecHandle AbilitySpecHandle;

	UPROPERTY()
	float AbilityLevel = 0.f;

	UPROPERTY()
	UTexture2D* AbilityTexture = nullptr;

	UPROPERTY()
	FGameplayTagContainer SlotTags;
};

UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class UAbilitySlotInterface : public UInterface
{
	GENERATED_BODY()
};

class DEMO_API IAbilitySlotInterface
{
	GENERATED_BODY()

public:
	virtual void InitAbilitySlotData(FGameplayAbilitySpecHandle AbilitySpecHandle, FAbilityData& AbilityData, FGameplayTagContainer& AbilityTags) = 0;
};
