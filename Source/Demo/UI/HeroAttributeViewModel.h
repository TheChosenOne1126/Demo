// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "HeroAttributeViewModel.generated.h"

UCLASS()
class DEMO_API UHeroAttributeViewModel : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	float GetXp() const;

	void SetXp(float NewXp);

	float GetStrength() const;

	void SetStrength(float NewStrength);

	float GetExtraStrength() const;

	void SetExtraStrength(float NewExtraStrength);

	float GetIntelligence() const;

	void SetIntelligence(float NewIntelligence);

	float GetExtraIntelligence() const;

	void SetExtraIntelligence(float NewExtraIntelligence);

	float GetAgility() const;

	void SetAgility(float NewAgility);

	float GetExtraAgility() const;

	void SetExtraAgility(float NewExtraAgility);

private:
	UPROPERTY(FieldNotify, Setter, Getter)
	float Xp;

	UPROPERTY(FieldNotify, Setter, Getter)
	float Strength;

	UPROPERTY(FieldNotify, Setter, Getter)
	float ExtraStrength;

	UPROPERTY(FieldNotify, Setter, Getter)
	float Intelligence;

	UPROPERTY(FieldNotify, Setter, Getter)
	float ExtraIntelligence;

	UPROPERTY(FieldNotify, Setter, Getter)
	float Agility;

	UPROPERTY(FieldNotify, Setter, Getter)
	float ExtraAgility;
};
