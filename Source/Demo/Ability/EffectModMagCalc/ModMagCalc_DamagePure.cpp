// Fill out your copyright notice in the Description page of Project Settings.

#include "ModMagCalc_DamagePure.h"

float UModMagCalc_DamagePure::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	FAggregatorEvaluateParameters EvaluateParams;
	EvaluateParams.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	EvaluateParams.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	
	return GetFixedDamageValue(Spec, EvaluateParams) + GetDamageAttributeValue(Spec, EvaluateParams) + GetPerHpDamageValue(Spec, EvaluateParams);
}
