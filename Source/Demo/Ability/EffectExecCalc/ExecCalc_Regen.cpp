// Fill out your copyright notice in the Description page of Project Settings.

#include "ExecCalc_Regen.h"
#include "Attribute/BaseAttributeSet.h"
#include "Global/Statics.h"

UExecCalc_Regen::UExecCalc_Regen()
{
	HpRegenCaptureDef.AttributeToCapture = UBaseAttributeSet::GetHpRegenAttribute();
	HpRegenCaptureDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Source;
	RelevantAttributesToCapture.Emplace(HpRegenCaptureDef);

	MpRegenCaptureDef.AttributeToCapture = UBaseAttributeSet::GetMpRegenAttribute();
	MpRegenCaptureDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Source;
	RelevantAttributesToCapture.Emplace(MpRegenCaptureDef);

#if WITH_EDITORONLY_DATA
	InvalidScopedModifierAttributes.Emplace(HpRegenCaptureDef);
	InvalidScopedModifierAttributes.Emplace(MpRegenCaptureDef);
#endif
}

void UExecCalc_Regen::Execute_Implementation(
	const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	Super::Execute_Implementation(ExecutionParams, OutExecutionOutput);

	FAggregatorEvaluateParameters EvaluateParams;
	EvaluateParams.SourceTags = ExecutionParams.GetOwningSpec().CapturedSourceTags.GetAggregatedTags();
	EvaluateParams.TargetTags = ExecutionParams.GetOwningSpec().CapturedTargetTags.GetAggregatedTags();

	float HpRegen = 0.f;
	if (!ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(HpRegenCaptureDef, EvaluateParams, HpRegen))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: Failed to Calculate HpRegen Captured"), __FUNCTION__));
		return;
	}

	if (HpRegen > 0.f)
	{
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UBaseAttributeSet::GetHpAttribute(), EGameplayModOp::AddBase, HpRegen));
	}

	float MpRegen = 0.f;
	if (!ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(MpRegenCaptureDef, EvaluateParams, MpRegen))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: Failed to Calculate MpRegen Captured"), __FUNCTION__));
		return;
	}

	if (MpRegen > 0.f)
	{
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UBaseAttributeSet::GetMpAttribute(), EGameplayModOp::AddBase, MpRegen));
	}
}
