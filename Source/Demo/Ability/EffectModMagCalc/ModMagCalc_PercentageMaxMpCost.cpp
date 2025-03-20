// Fill out your copyright notice in the Description page of Project Settings.

#include "ModMagCalc_PercentageMaxMpCost.h"
#include "Attribute/BaseAttributeSet.h"
#include "Global/GlobalTags.h"
#include "Global/Statics.h"

UModMagCalc_PercentageMaxMpCost::UModMagCalc_PercentageMaxMpCost()
{
	MaxMpCaptureDef.AttributeToCapture = UBaseAttributeSet::GetMaxHpAttribute();
	MaxMpCaptureDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Source;
	RelevantAttributesToCapture.Emplace(MaxMpCaptureDef);
}

float UModMagCalc_PercentageMaxMpCost::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	const float MaxMpPercentage = Spec.GetSetByCallerMagnitude(GlobalTags::SetByCallerPerMaxMpCostTag, false);
	if (MaxMpPercentage <= 0.f)
	{
		return 0.f;
	}

	FAggregatorEvaluateParameters EvaluateParams;
	EvaluateParams.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	EvaluateParams.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	float MaxMp = 0.f;
	if (!GetCapturedAttributeMagnitude(MaxMpCaptureDef, Spec, EvaluateParams, MaxMp))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: Failed to Capture MaxMp Attribute"), __FUNCTION__));
	}

	return MaxMp * MaxMpPercentage;
}
