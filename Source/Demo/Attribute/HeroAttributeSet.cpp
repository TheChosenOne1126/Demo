// Fill out your copyright notice in the Description page of Project Settings.

#include "HeroAttributeSet.h"
#include "Net/UnrealNetwork.h"

void UHeroAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, Xp, COND_None, REPNOTIFY_Always)
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, Strength, COND_None, REPNOTIFY_Always)
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, ExtraStrength, COND_None, REPNOTIFY_Always)
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, Intelligence, COND_None, REPNOTIFY_Always)
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, ExtraIntelligence, COND_None, REPNOTIFY_Always)
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, Agility, COND_None, REPNOTIFY_Always)
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, ExtraAgility, COND_None, REPNOTIFY_Always)
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, BaseSp, COND_None, REPNOTIFY_Always)
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, UltimateSp, COND_None, REPNOTIFY_Always)
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, MaxSp, COND_None, REPNOTIFY_Always)
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, DamageReduction, COND_None, REPNOTIFY_Always)
}

void UHeroAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetUltimateSpAttribute())
	{
		NewValue = FMath::Max(NewValue, 0.0f);
	}
	else if (Attribute == GetBaseSpAttribute())
	{
		NewValue = FMath::Max(NewValue, 0.0f);
	}
	else if (Attribute == GetMaxSpAttribute())
	{
		NewValue = FMath::Max(NewValue, 0.0f);
	}
}

void UHeroAttributeSet::OnRep_Xp(const FGameplayAttributeData& OldXp) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, Xp, OldXp)
}

void UHeroAttributeSet::OnRep_Strength(const FGameplayAttributeData& OldStrength) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, Strength, OldStrength)
}

void UHeroAttributeSet::OnRep_ExtraStrength(const FGameplayAttributeData& OldExtraStrength) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, ExtraStrength, OldExtraStrength)
}

void UHeroAttributeSet::OnRep_Intelligence(const FGameplayAttributeData& OldIntelligence) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, Intelligence, OldIntelligence)
}

void UHeroAttributeSet::OnRep_ExtraIntelligence(const FGameplayAttributeData& OldExtraIntelligence) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, ExtraIntelligence, OldExtraIntelligence)
}

void UHeroAttributeSet::OnRep_Agility(const FGameplayAttributeData& OldAgility) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, Agility, OldAgility)
}

void UHeroAttributeSet::OnRep_ExtraAgility(const FGameplayAttributeData& OldExtraAgility) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, ExtraAgility, OldExtraAgility)
}

void UHeroAttributeSet::OnRep_BaseAbilityPoint(const FGameplayAttributeData& OldBaseAbilityPoint) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, BaseSp, OldBaseAbilityPoint)
}

void UHeroAttributeSet::OnRep_UltimateAbilityPoint(const FGameplayAttributeData& OldUltimateAbilityPoint) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, UltimateSp, OldUltimateAbilityPoint)
}

void UHeroAttributeSet::OnRep_MaxAbilityPoint(const FGameplayAttributeData& OldMaxAbilityPoint) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, MaxSp, OldMaxAbilityPoint)
}

void UHeroAttributeSet::OnRep_DamageReduction(const FGameplayAttributeData& OldDamageReduction) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, DamageReduction, OldDamageReduction)
}
