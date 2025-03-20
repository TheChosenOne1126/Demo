// Fill out your copyright notice in the Description page of Project Settings.

#include "ModMagCalc_DamageMagic.h"
#include "Attribute/BaseAttributeSet.h"
#include "Global/Statics.h"

UModMagCalc_DamageMagic::UModMagCalc_DamageMagic()
{
	MagicResistCaptureDef.AttributeToCapture = UBaseAttributeSet::GetMagicResistAttribute();
	MagicResistCaptureDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	RelevantAttributesToCapture.Emplace(MagicResistCaptureDef);

	ExtraMagicResistCaptureDef.AttributeToCapture = UBaseAttributeSet::GetExtraMagicResistAttribute();
	ExtraMagicResistCaptureDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	RelevantAttributesToCapture.Emplace(ExtraMagicResistCaptureDef);
}

float UModMagCalc_DamageMagic::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	FAggregatorEvaluateParameters EvaluateParams;
	EvaluateParams.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	EvaluateParams.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	
	float MagicResist = 0.f;
	if (!GetCapturedAttributeMagnitude(MagicResistCaptureDef, Spec, EvaluateParams, MagicResist))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: Failed to to Capture MagicResist Attribute"), __FUNCTION__));
	}

	float ExtraMagicResist = 0.f;
	if (!GetCapturedAttributeMagnitude(ExtraMagicResistCaptureDef, Spec, EvaluateParams, ExtraMagicResist))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: Failed to to Capture ExtraMagicResist Attribute"), __FUNCTION__));
	}

	const float OriginalDamage = GetFixedDamageValue(Spec, EvaluateParams) + GetDamageAttributeValue(Spec, EvaluateParams) + GetPerHpDamageValue(Spec, EvaluateParams);
	return OriginalDamage * (1.f - MagicResist - ExtraMagicResist);
}
