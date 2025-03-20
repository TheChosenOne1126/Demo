// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseAttributeSet.h"
#include "Net/UnrealNetwork.h"

void UBaseAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, Hp, COND_None, REPNOTIFY_Always)
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, MaxHp, COND_None, REPNOTIFY_Always)
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, HpRegen, COND_None, REPNOTIFY_Always)
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, Mp, COND_None, REPNOTIFY_Always)
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, MaxMp, COND_None, REPNOTIFY_Always)
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, MpRegen, COND_None, REPNOTIFY_Always)
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, Lv, COND_None, REPNOTIFY_Always)
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, MaxLv, COND_None, REPNOTIFY_Always)
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, Damage, COND_None, REPNOTIFY_Always)
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, ExtraDamage, COND_None, REPNOTIFY_Always)
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, Armor, COND_None, REPNOTIFY_Always)
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, ExtraArmor, COND_None, REPNOTIFY_Always)
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, MagicResist, COND_None, REPNOTIFY_Always)
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, ExtraMagicResist, COND_None, REPNOTIFY_Always)
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, Evasion, COND_None, REPNOTIFY_Always)
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, AttackSpeed, COND_None, REPNOTIFY_Always)
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

void UBaseAttributeSet::OnRep_ExtraDamage(const FGameplayAttributeData& OldExtraDamage) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, ExtraDamage, OldExtraDamage)
}

void UBaseAttributeSet::OnRep_Armor(const FGameplayAttributeData& OldArmor) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, Armor, OldArmor)
}

void UBaseAttributeSet::OnRep_ExtraArmor(const FGameplayAttributeData& OldExtraArmor) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, ExtraArmor, OldExtraArmor)
}

void UBaseAttributeSet::OnRep_MagicResist(const FGameplayAttributeData& OldMagicResist) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, MagicResist, OldMagicResist)
}

void UBaseAttributeSet::OnRep_ExtraMagicResist(const FGameplayAttributeData& OldExtraMagicResist) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, ExtraMagicResist, OldExtraMagicResist)	
}

void UBaseAttributeSet::OnRep_Evasion(const FGameplayAttributeData& OldEvasion) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, Evasion, OldEvasion)
}

void UBaseAttributeSet::OnRep_AttackSpeed(const FGameplayAttributeData& OldAttackSpeed) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, AttackSpeed, OldAttackSpeed)
}
