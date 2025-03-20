// Fill out your copyright notice in the Description page of Project Settings.

#include "ModMagCalc_DamagePhysical.h"
#include "Attribute/BaseAttributeSet.h"
#include "Global/DemoSettings.h"
#include "Global/Statics.h"

UModMagCalc_DamagePhysical::UModMagCalc_DamagePhysical()
{
	ArmorCaptureDef.AttributeToCapture = UBaseAttributeSet::GetArmorAttribute();
	ArmorCaptureDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	RelevantAttributesToCapture.Emplace(ArmorCaptureDef);

	ExtraArmorCaptureDef.AttributeToCapture = UBaseAttributeSet::GetExtraArmorAttribute();
	ExtraArmorCaptureDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	RelevantAttributesToCapture.Emplace(ExtraArmorCaptureDef);
}

float UModMagCalc_DamagePhysical::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	const UDemoSettings* DemoSettings = GetDefault<UDemoSettings>();
	if (!IsValid(DemoSettings))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid DemoSettings"), __FUNCTION__));
		return 0.f;
	}

	FAggregatorEvaluateParameters EvaluateParams;
	EvaluateParams.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	EvaluateParams.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	
	float Armor = 0.f;
	if (!GetCapturedAttributeMagnitude(ArmorCaptureDef, Spec, EvaluateParams, Armor))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: Failed to to Capture Armor Attribute"), __FUNCTION__));
	}

	float ExtraArmor = 0.f;
	if (!GetCapturedAttributeMagnitude(ExtraArmorCaptureDef, Spec, EvaluateParams, ExtraArmor))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: Failed to to Capture ExtraArmor Attribute"), __FUNCTION__));
	}

	const float OriginalDamage = GetFixedDamageValue(Spec, EvaluateParams) + GetDamageAttributeValue(Spec, EvaluateParams) + GetPerHpDamageValue(Spec, EvaluateParams);
	return OriginalDamage * (1.f - DemoSettings->ArmorCoe * (Armor + ExtraArmor) / (1.f + DemoSettings->ArmorCoe * (Armor + ExtraArmor)));
}
