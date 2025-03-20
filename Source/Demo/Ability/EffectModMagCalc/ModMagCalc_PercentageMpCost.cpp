// Fill out your copyright notice in the Description page of Project Settings.

#include "ModMagCalc_PercentageMpCost.h"
#include "Attribute/BaseAttributeSet.h"
#include "Global/GlobalTags.h"
#include "Global/Statics.h"

UModMagCalc_PercentageMpCost::UModMagCalc_PercentageMpCost()
{
	MpCaptureDef.AttributeToCapture = UBaseAttributeSet::GetMpAttribute();
	MpCaptureDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Source;
	RelevantAttributesToCapture.Emplace(MpCaptureDef);
}

float UModMagCalc_PercentageMpCost::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	const float MpPercentage = Spec.GetSetByCallerMagnitude(GlobalTags::SetByCallerPerMpCostTag, false);
	if (MpPercentage <= 0.f)
	{
		return 0.f;
	}

	FAggregatorEvaluateParameters EvaluateParams;
	EvaluateParams.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	EvaluateParams.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	float Mp = 0.f;
	if (!GetCapturedAttributeMagnitude(MpCaptureDef, Spec, EvaluateParams, Mp))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: Failed to Capture MaxMp Attribute"), __FUNCTION__));
	}

	return Mp * MpPercentage;
}
