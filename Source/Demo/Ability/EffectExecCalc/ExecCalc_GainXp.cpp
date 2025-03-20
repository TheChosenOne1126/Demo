// Fill out your copyright notice in the Description page of Project Settings.

#include "ExecCalc_GainXp.h"
#include "Attribute/BaseAttributeSet.h"
#include "Attribute/HeroAttributeSet.h"
#include "Global/DemoSettings.h"
#include "Global/Statics.h"

UExecCalc_GainXp::UExecCalc_GainXp()
{
	XpCaptureDef.AttributeToCapture = UHeroAttributeSet::GetXpAttribute();
	XpCaptureDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Source;
	RelevantAttributesToCapture.Emplace(XpCaptureDef);
	
	LvCaptureDef.AttributeToCapture = UBaseAttributeSet::GetLvAttribute();
	LvCaptureDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Source;
	RelevantAttributesToCapture.Emplace(LvCaptureDef);

	MaxSpCaptureDef.AttributeToCapture = UHeroAttributeSet::GetMaxSpAttribute();
	MaxSpCaptureDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Source;
	RelevantAttributesToCapture.Emplace(MaxSpCaptureDef);

#if WITH_EDITORONLY_DATA
	InvalidScopedModifierAttributes.Emplace(LvCaptureDef);
	InvalidScopedModifierAttributes.Emplace(MaxSpCaptureDef);
#endif
}

void UExecCalc_GainXp::Execute_Implementation(
	const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	Super::Execute_Implementation(ExecutionParams, OutExecutionOutput);

	FAggregatorEvaluateParameters EvaluateParams;
	EvaluateParams.SourceTags = ExecutionParams.GetOwningSpec().CapturedSourceTags.GetAggregatedTags();
	EvaluateParams.TargetTags = ExecutionParams.GetOwningSpec().CapturedTargetTags.GetAggregatedTags();

	float XpValue = 0.f;
	if (!ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(XpCaptureDef, EvaluateParams, XpValue))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: Failed to Calculate Xp Captured"), __FUNCTION__));
		return;
	}
	
	const float LvDelta = FMath::FloorToInt(XpValue);
	XpValue = FMath::Frac(XpValue);

	UStatics::Log(this, ELogType::Verbose, FString::Printf(TEXT("%hs: XpValue = %f"), __FUNCTION__, XpValue));
	OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(XpCaptureDef.AttributeToCapture, EGameplayModOp::Override, XpValue));
	
	if (LvDelta > 0.f)
	{
		UStatics::Log(this, ELogType::Verbose, FString::Printf(TEXT("%hs: LvDelta = %f"), __FUNCTION__, LvDelta));
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(LvCaptureDef.AttributeToCapture, EGameplayModOp::AddBase, LvDelta));
	}
	
	const UDemoSettings* DemoSettings = GetDefault<UDemoSettings>();
	if (!IsValid(DemoSettings))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid DemoSettings"), __FUNCTION__));
		return;
	}

	float CurLvValue = 0.f;
	if (!ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(LvCaptureDef, EvaluateParams, CurLvValue))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: Failed to Calculate Lv Captured"), __FUNCTION__));
		return;
	}
	
	float UltimateSpDelta = 0.f;
	for (const int32& GainExtraSpPawnLevel : DemoSettings->GainExtraAbilityPointPawnLevelSet)
	{
		if (CurLvValue < GainExtraSpPawnLevel && CurLvValue + LvDelta >= GainExtraSpPawnLevel)
		{
			UltimateSpDelta++;
		}
	}

	if (UltimateSpDelta > 0.f)
	{
		UStatics::Log(this, ELogType::Verbose, FString::Printf(TEXT("%hs: UltimateAbilityPointDelta = %f"),
			__FUNCTION__, UltimateSpDelta));
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UHeroAttributeSet::GetUltimateSpAttribute(),
			EGameplayModOp::AddBase, UltimateSpDelta));
	}

	float CurMaxSpValue = 0.f;
	if (!ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(MaxSpCaptureDef, EvaluateParams, CurMaxSpValue))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: Failed to Calculate MaxSp Captured"), __FUNCTION__));
		return;
	}
	
	const float BaseSpDelta = FMath::Min(LvDelta - UltimateSpDelta, CurMaxSpValue);
	if (BaseSpDelta > 0.f)
	{
		UStatics::Log(this, ELogType::Verbose, FString::Printf(TEXT("%hs: BaseAbilityPointDelta = %f"), __FUNCTION__, BaseSpDelta));
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UHeroAttributeSet::GetBaseSpAttribute(),
			EGameplayModOp::AddBase, BaseSpDelta));
	}
}
