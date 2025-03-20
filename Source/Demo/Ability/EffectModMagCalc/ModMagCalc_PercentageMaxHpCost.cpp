// Fill out your copyright notice in the Description page of Project Settings.

#include "ModMagCalc_PercentageMaxHpCost.h"
#include "Attribute/BaseAttributeSet.h"
#include "Global/GlobalTags.h"
#include "Global/Statics.h"

UModMagCalc_PercentageMaxHpCost::UModMagCalc_PercentageMaxHpCost()
{
	MaxHpCaptureDef.AttributeToCapture = UBaseAttributeSet::GetMaxHpAttribute();
	MaxHpCaptureDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Source;
	RelevantAttributesToCapture.Emplace(MaxHpCaptureDef);
}

float UModMagCalc_PercentageMaxHpCost::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	const float MaxHpPercentage = Spec.GetSetByCallerMagnitude(GlobalTags::SetByCallerPerMaxHpCostTag, false);
	if (MaxHpPercentage <= 0.f)
	{
		return 0.f;
	}

	FAggregatorEvaluateParameters EvaluateParams;
	EvaluateParams.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	EvaluateParams.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	float MaxHp = 0.f;
	if (!GetCapturedAttributeMagnitude(MaxHpCaptureDef, Spec, EvaluateParams, MaxHp))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: Failed to Capture MaxHp Attribute"), __FUNCTION__));
	}

	return MaxHp * MaxHpPercentage;
}
