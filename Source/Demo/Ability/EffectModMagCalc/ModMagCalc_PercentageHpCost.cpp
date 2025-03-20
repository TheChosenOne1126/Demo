// Fill out your copyright notice in the Description page of Project Settings.

#include "ModMagCalc_PercentageHpCost.h"
#include "Attribute/BaseAttributeSet.h"
#include "Global/GlobalTags.h"
#include "Global/Statics.h"

UModMagCalc_PercentageHpCost::UModMagCalc_PercentageHpCost()
{
	HpCaptureDef.AttributeToCapture = UBaseAttributeSet::GetHpAttribute();
	HpCaptureDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Source;
	RelevantAttributesToCapture.Emplace(HpCaptureDef);
}

float UModMagCalc_PercentageHpCost::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	const float HpPercentage = Spec.GetSetByCallerMagnitude(GlobalTags::SetByCallerPerHpCostTag, false);
	if (HpPercentage <= 0.f)
	{
		return 0.f;
	}

	FAggregatorEvaluateParameters EvaluateParams;
	EvaluateParams.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	EvaluateParams.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	float Hp = 0.f;
	if (!GetCapturedAttributeMagnitude(HpCaptureDef, Spec, EvaluateParams, Hp))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: Failed to Capture Hp Attribute"), __FUNCTION__));
	}

	return Hp * HpPercentage;
}
