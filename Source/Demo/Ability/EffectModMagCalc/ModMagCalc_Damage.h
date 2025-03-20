// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "ModMagCalc_Damage.generated.h"

UCLASS(Abstract)
class DEMO_API UModMagCalc_Damage : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()

public:
	UModMagCalc_Damage();

protected:
	float GetDamageAttributeValue(const FGameplayEffectSpec& Spec, const FAggregatorEvaluateParameters& EvaluationParams) const;

	float GetFixedDamageValue(const FGameplayEffectSpec& Spec, const FAggregatorEvaluateParameters& EvaluationParams) const;

	float GetPerHpDamageValue(const FGameplayEffectSpec& Spec, const FAggregatorEvaluateParameters& EvaluationParams) const;

private:
	float GetDamageReductionAttributeValue(const FGameplayEffectSpec& Spec, const FAggregatorEvaluateParameters& EvaluationParams) const;

	UPROPERTY()
	FGameplayEffectAttributeCaptureDefinition DamageCaptureDef;
	
	UPROPERTY()
	FGameplayEffectAttributeCaptureDefinition ExtraDamageCaptureDef;

	UPROPERTY()
	FGameplayEffectAttributeCaptureDefinition DamageReductionCaptureDef;

	UPROPERTY()
	FGameplayEffectAttributeCaptureDefinition HpCaptureDef;
	
	UPROPERTY()
	FGameplayEffectAttributeCaptureDefinition MaxHpCaptureDef;
};
