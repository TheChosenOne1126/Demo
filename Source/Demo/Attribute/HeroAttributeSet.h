// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "Component/DAbilitySystemComponent.h"
#include "HeroAttributeSet.generated.h"

UCLASS()
class DEMO_API UHeroAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	ATTRIBUTE_ACCESSORS(ThisClass, Xp)
	ATTRIBUTE_ACCESSORS(ThisClass, Strength)
	ATTRIBUTE_ACCESSORS(ThisClass, ExtraStrength)
	ATTRIBUTE_ACCESSORS(ThisClass, Intelligence)
	ATTRIBUTE_ACCESSORS(ThisClass, ExtraIntelligence)
	ATTRIBUTE_ACCESSORS(ThisClass, Agility)
	ATTRIBUTE_ACCESSORS(ThisClass, ExtraAgility)
	ATTRIBUTE_ACCESSORS(ThisClass, BaseSp)
	ATTRIBUTE_ACCESSORS(ThisClass, UltimateSp)
	ATTRIBUTE_ACCESSORS(ThisClass, MaxSp)
	ATTRIBUTE_ACCESSORS(ThisClass, DamageReduction)

protected:
	UFUNCTION()
	void OnRep_Xp(const FGameplayAttributeData& OldXp) const;

	UFUNCTION()
	void OnRep_Strength(const FGameplayAttributeData& OldStrength) const;

	UFUNCTION()
	void OnRep_ExtraStrength(const FGameplayAttributeData& OldExtraStrength) const;

	UFUNCTION()
	void OnRep_Intelligence(const FGameplayAttributeData& OldIntelligence) const;

	UFUNCTION()
	void OnRep_ExtraIntelligence(const FGameplayAttributeData& OldExtraIntelligence) const;

	UFUNCTION()
	void OnRep_Agility(const FGameplayAttributeData& OldAgility) const;

	UFUNCTION()
	void OnRep_ExtraAgility(const FGameplayAttributeData& OldExtraAgility) const;

	UFUNCTION()
	void OnRep_BaseAbilityPoint(const FGameplayAttributeData& OldBaseAbilityPoint) const;

	UFUNCTION()
	void OnRep_UltimateAbilityPoint(const FGameplayAttributeData& OldUltimateAbilityPoint) const;

	UFUNCTION()
	void OnRep_MaxAbilityPoint(const FGameplayAttributeData& OldMaxAbilityPoint) const;

	UFUNCTION()
	void OnRep_DamageReduction(const FGameplayAttributeData& OldDamageReduction) const;

private:
	UPROPERTY(ReplicatedUsing = OnRep_Xp)
	FGameplayAttributeData Xp;

	UPROPERTY(ReplicatedUsing = OnRep_Strength)
	FGameplayAttributeData Strength;

	UPROPERTY(ReplicatedUsing = OnRep_ExtraStrength)
	FGameplayAttributeData ExtraStrength;

	UPROPERTY(replicatedUsing = OnRep_Intelligence)
	FGameplayAttributeData Intelligence;

	UPROPERTY(replicatedUsing = OnRep_ExtraIntelligence)
	FGameplayAttributeData ExtraIntelligence;

	UPROPERTY(ReplicatedUsing = OnRep_Agility)
	FGameplayAttributeData Agility;

	UPROPERTY(ReplicatedUsing = OnRep_ExtraAgility)
	FGameplayAttributeData ExtraAgility;

	UPROPERTY(ReplicatedUsing = OnRep_BaseAbilityPoint)
	FGameplayAttributeData BaseSp;

	UPROPERTY(ReplicatedUsing = OnRep_UltimateAbilityPoint)
	FGameplayAttributeData UltimateSp;

	UPROPERTY(ReplicatedUsing = OnRep_MaxAbilityPoint)
	FGameplayAttributeData MaxSp;

	UPROPERTY(ReplicatedUsing = OnRep_DamageReduction)
	FGameplayAttributeData DamageReduction;
};
