// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseAttributeSet.h"
#include "Net/UnrealNetwork.h"

void UBaseAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, Hp, Params);
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, MaxHp, Params)
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, HpRegen, Params)
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, Mp, Params)
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, MaxMp, Params)
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, MpRegen, Params)
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, Lv, Params)
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, MaxLv, Params)
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, Damage, Params)
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, Armor, Params)
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, AttackSpeed, Params)
}

void UBaseAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetHpAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHp());
	}
	else if (Attribute == GetMpAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxMp());
	}
	else if (Attribute == GetLvAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 1.0f, GetMaxLv());
		NewValue = FMath::Max(GetLv(), NewValue);
	}
}

void UBaseAttributeSet::OnRep_Hp(const FGameplayAttributeData& OldHp) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, Hp, OldHp)
}

void UBaseAttributeSet::OnRep_MaxHp(const FGameplayAttributeData& OldHpMax) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, MaxHp, OldHpMax)
}

void UBaseAttributeSet::OnRep_HpRegen(const FGameplayAttributeData& OldHpRegen) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, HpRegen, OldHpRegen)
}

void UBaseAttributeSet::OnRep_Mp(const FGameplayAttributeData& OldMp) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, Mp, OldMp)
}

void UBaseAttributeSet::OnRep_MaxMp(const FGameplayAttributeData& OldMpMax) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, MaxMp, OldMpMax)
}

void UBaseAttributeSet::OnRep_MpRegen(const FGameplayAttributeData& OldMpRegen) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, MpRegen, OldMpRegen)
}

void UBaseAttributeSet::OnRep_Lv(const FGameplayAttributeData& OldLv) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, Lv, OldLv)
}

void UBaseAttributeSet::OnRep_MaxLv(const FGameplayAttributeData& OldLvMax) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, MaxLv, OldLvMax)
}

void UBaseAttributeSet::OnRep_Damage(const FGameplayAttributeData& OldDamage) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, Damage, OldDamage)
}

void UBaseAttributeSet::OnRep_Armor(const FGameplayAttributeData& OldArmor) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, Armor, OldArmor)
}

void UBaseAttributeSet::OnRep_AttackSpeed(const FGameplayAttributeData& OldAttackSpeed) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, AttackSpeed, OldAttackSpeed)
}
