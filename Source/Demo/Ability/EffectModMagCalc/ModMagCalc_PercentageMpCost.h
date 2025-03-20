// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "ModMagCalc_PercentageMpCost.generated.h"

UCLASS()
class DEMO_API UModMagCalc_PercentageMpCost : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()

public:
	UModMagCalc_PercentageMpCost();

	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;

private:
	UPROPERTY()
	FGameplayEffectAttributeCaptureDefinition MpCaptureDef;
};
