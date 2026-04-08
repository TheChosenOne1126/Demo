// Fill out your copyright notice in the Description page of Project Settings.

#include "ModMagCalc_Damage.h"
#include "Attribute/BaseAttributeSet.h"
#include "Attribute/HeroAttributeSet.h"

UModMagCalc_Damage::UModMagCalc_Damage()
{
	DamageCaptureDef.AttributeToCapture = UBaseAttributeSet::GetDamageAttribute();
	DamageCaptureDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Source;
	RelevantAttributesToCapture.Emplace(DamageCaptureDef);

	HpCaptureDef.AttributeToCapture = UBaseAttributeSet::GetHpAttribute();
	HpCaptureDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	RelevantAttributesToCapture.Emplace(HpCaptureDef);
}

float UModMagCalc_Damage::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	return Super::CalculateBaseMagnitude_Implementation(Spec);
}
