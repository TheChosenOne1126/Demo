// Fill out your copyright notice in the Description page of Project Settings.

#include "ModMagCalc_Damage.h"
#include "Attribute/BaseAttributeSet.h"
#include "Attribute/HeroAttributeSet.h"
#include "Global/GlobalTags.h"
#include "Global/Statics.h"

UModMagCalc_Damage::UModMagCalc_Damage()
{
	DamageCaptureDef.AttributeToCapture = UBaseAttributeSet::GetDamageAttribute();
	DamageCaptureDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Source;
	RelevantAttributesToCapture.Emplace(DamageCaptureDef);

	ExtraDamageCaptureDef.AttributeToCapture = UBaseAttributeSet::GetExtraDamageAttribute();
	ExtraDamageCaptureDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Source;
	RelevantAttributesToCapture.Emplace(ExtraDamageCaptureDef);

	DamageReductionCaptureDef.AttributeToCapture = UHeroAttributeSet::GetDamageReductionAttribute();
	DamageReductionCaptureDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	RelevantAttributesToCapture.Emplace(DamageReductionCaptureDef);

	HpCaptureDef.AttributeToCapture = UBaseAttributeSet::GetHpAttribute();
	HpCaptureDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	RelevantAttributesToCapture.Emplace(HpCaptureDef);

	MaxHpCaptureDef.AttributeToCapture = UBaseAttributeSet::GetMaxHpAttribute();
	MaxHpCaptureDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	RelevantAttributesToCapture.Emplace(MaxHpCaptureDef);
}

float UModMagCalc_Damage::GetDamageAttributeValue(const FGameplayEffectSpec& Spec, const FAggregatorEvaluateParameters& EvaluationParams) const
{
	float PerDamage = Spec.GetSetByCallerMagnitude(GlobalTags::SetByCallerPerDamageAttributeTag);
	if (PerDamage > 0.f)
	{
		float Damage = 0.f;
		if (!GetCapturedAttributeMagnitude(DamageCaptureDef, Spec, EvaluationParams, Damage))
		{
			UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: Failed to to Capture Damage Attribute"), __FUNCTION__));
		}

		PerDamage *= Damage;
	}

	float PerExtraDamage = Spec.GetSetByCallerMagnitude(GlobalTags::SetByCallerPerExtraDamageAttributeTag);
	if (PerExtraDamage > 0.f)
	{
		float ExtraDamage = 0.f;
		if (!GetCapturedAttributeMagnitude(ExtraDamageCaptureDef, Spec, EvaluationParams, ExtraDamage))
		{
			UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: Failed to to Capture ExtraDamage Attribute"), __FUNCTION__));
		}

		PerExtraDamage *= ExtraDamage;
	}

	return (PerDamage + PerExtraDamage) * (1.f - GetDamageReductionAttributeValue(Spec, EvaluationParams));
}

float UModMagCalc_Damage::GetFixedDamageValue(const FGameplayEffectSpec& Spec, const FAggregatorEvaluateParameters& EvaluationParams) const
{
	float FixedDamageValue = Spec.GetSetByCallerMagnitude(GlobalTags::SetByCallerFixedDamageTag);
	if (FixedDamageValue > 0.f)
	{
		FixedDamageValue *= 1.f - GetDamageReductionAttributeValue(Spec, EvaluationParams);
	}

	return FixedDamageValue;
}

float UModMagCalc_Damage::GetPerHpDamageValue(const FGameplayEffectSpec& Spec, const FAggregatorEvaluateParameters& EvaluationParams) const
{
	float PerHpDamageValue = Spec.GetSetByCallerMagnitude(GlobalTags::SetByCallerPerHpDamageTag);
	if (PerHpDamageValue > 0.f)
	{
		float Hp = 0.f;
		if (!GetCapturedAttributeMagnitude(DamageCaptureDef, Spec, EvaluationParams, Hp))
		{
			UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: Failed to to Capture Hp Attribute"), __FUNCTION__));
		}

		PerHpDamageValue *= Hp;
	}

	float PerMaxHpDamageValue = Spec.GetSetByCallerMagnitude(GlobalTags::SetByCallerPerMaxHpDamageTag);
	if (PerMaxHpDamageValue > 0.f)
	{
		float MaxHp = 0.f;
		if (!GetCapturedAttributeMagnitude(MaxHpCaptureDef, Spec, EvaluationParams, MaxHp))
		{
			UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: Failed to to Capture Max Hp Attribute"), __FUNCTION__));
		}

		PerMaxHpDamageValue *= MaxHp;
	}

	return (PerHpDamageValue + PerMaxHpDamageValue) * (1.f - GetDamageReductionAttributeValue(Spec, EvaluationParams));
}

float UModMagCalc_Damage::GetDamageReductionAttributeValue(const FGameplayEffectSpec& Spec, const FAggregatorEvaluateParameters& EvaluationParams) const
{
	float DamageReduction = 0.f;
	if (!GetCapturedAttributeMagnitude(DamageCaptureDef, Spec, EvaluationParams, DamageReduction))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: Failed to to Capture DamageReduction Attribute"), __FUNCTION__));
	}

	return DamageReduction;
}