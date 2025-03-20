// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ModMagCalc_Damage.h"
#include "ModMagCalc_DamageMagic.generated.h"

UCLASS()
class DEMO_API UModMagCalc_DamageMagic : public UModMagCalc_Damage
{
	GENERATED_BODY()

public:
	UModMagCalc_DamageMagic();
	
	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;

private:
	UPROPERTY()
	FGameplayEffectAttributeCaptureDefinition MagicResistCaptureDef;
	
	UPROPERTY()
	FGameplayEffectAttributeCaptureDefinition ExtraMagicResistCaptureDef;
};
