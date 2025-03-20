// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ModMagCalc_Damage.h"
#include "ModMagCalc_DamagePure.generated.h"

UCLASS()
class DEMO_API UModMagCalc_DamagePure : public UModMagCalc_Damage
{
	GENERATED_BODY()

public:
	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
};
