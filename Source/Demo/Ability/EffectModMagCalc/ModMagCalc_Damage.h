// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "ModMagCalc_Damage.generated.h"

UCLASS()
class DEMO_API UModMagCalc_Damage : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()

public:
	UModMagCalc_Damage();

protected:
	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
	
private:
	UPROPERTY()
	FGameplayEffectAttributeCaptureDefinition DamageCaptureDef;

	UPROPERTY()
	FGameplayEffectAttributeCaptureDefinition HpCaptureDef;
};
