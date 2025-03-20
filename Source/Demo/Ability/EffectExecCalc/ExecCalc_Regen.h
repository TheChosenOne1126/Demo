// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "ExecCalc_Regen.generated.h"

UCLASS()
class DEMO_API UExecCalc_Regen : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:
	UExecCalc_Regen();

	virtual void Execute_Implementation(
		const FGameplayEffectCustomExecutionParameters& ExecutionParams,
		FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;

private:
	UPROPERTY()
	FGameplayEffectAttributeCaptureDefinition HpRegenCaptureDef;

	UPROPERTY()
	FGameplayEffectAttributeCaptureDefinition MpRegenCaptureDef;
};
