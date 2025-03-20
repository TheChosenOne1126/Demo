// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseAttributeViewModel.h"

float UBaseAttributeViewModel::GetHp() const
{
	return Hp;
}

void UBaseAttributeViewModel::SetHp(const float NewHp)
{
	if (UE_MVVM_SET_PROPERTY_VALUE(Hp, NewHp))
	{
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetHpPercentage);
	}
}

float UBaseAttributeViewModel::GetMaxHp() const
{
	return MaxHp;
}

void UBaseAttributeViewModel::SetMaxHp(const float NewMaxHp)
{
	if (UE_MVVM_SET_PROPERTY_VALUE(MaxHp, NewMaxHp))
	{
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetHpPercentage);
	}
}

float UBaseAttributeViewModel::GetHpPercentage() const
{
	if (MaxHp > 0.f)
	{
		return Hp / MaxHp;
	}

	return 0.f;
}

float UBaseAttributeViewModel::GetMp() const
{
	return Mp;
}

void UBaseAttributeViewModel::SetMp(const float NewMp)
{
	if (UE_MVVM_SET_PROPERTY_VALUE(Mp, NewMp))
	{
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetMpPercentage);
	}
}

float UBaseAttributeViewModel::GetMaxMp() const
{
	return MaxMp;
}

void UBaseAttributeViewModel::SetMaxMp(const float NewMaxMp)
{
	if (UE_MVVM_SET_PROPERTY_VALUE(MaxMp, NewMaxMp))
	{
		UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetMpPercentage);
	}
}

float UBaseAttributeViewModel::GetHpRegen() const
{
	return HpRegen;
}

void UBaseAttributeViewModel::SetHpRegen(float NewHpRegen)
{
	UE_MVVM_SET_PROPERTY_VALUE(HpRegen, NewHpRegen);
}

float UBaseAttributeViewModel::GetMpPercentage() const
{
	if (MaxMp > 0.f)
	{
		return Mp / MaxMp;
	}

	return 0.f;
}

float UBaseAttributeViewModel::GetMpRegen() const
{
	return MpRegen;
}

void UBaseAttributeViewModel::SetMpRegen(float NewMpRegen)
{
	UE_MVVM_SET_PROPERTY_VALUE(MpRegen, NewMpRegen);
}

float UBaseAttributeViewModel::GetLv() const
{
	return Lv;
}

void UBaseAttributeViewModel::SetLv(const float NewLv)
{
	UE_MVVM_SET_PROPERTY_VALUE(Lv, NewLv);
}

float UBaseAttributeViewModel::GetDamage() const
{
	return Damage;
}

void UBaseAttributeViewModel::SetDamage(float NewDamage)
{
	UE_MVVM_SET_PROPERTY_VALUE(Damage, NewDamage);
}

float UBaseAttributeViewModel::GetExtraDamage() const
{
	return ExtraDamage;
}

void UBaseAttributeViewModel::SetExtraDamage(float NewExtraDamage)
{
	UE_MVVM_SET_PROPERTY_VALUE(ExtraDamage, NewExtraDamage);
}

float UBaseAttributeViewModel::GetArmor() const
{
	return Armor;
}

void UBaseAttributeViewModel::SetArmor(float NewArmor)
{
	UE_MVVM_SET_PROPERTY_VALUE(Armor, NewArmor);
}

float UBaseAttributeViewModel::GetExtraArmor() const
{
	return ExtraArmor;
}

void UBaseAttributeViewModel::SetExtraArmor(float NewExtraArmor)
{
	UE_MVVM_SET_PROPERTY_VALUE(ExtraArmor, NewExtraArmor);
}

float UBaseAttributeViewModel::GetMagicResist() const
{
	return MagicResist;
}

void UBaseAttributeViewModel::SetMagicResist(float NewMagicResist)
{
	UE_MVVM_SET_PROPERTY_VALUE(MagicResist, NewMagicResist);
}

float UBaseAttributeViewModel::GetExtraMagicResist() const
{
	return ExtraMagicResist;
}

void UBaseAttributeViewModel::SetExtraMagicResist(float NewExtraMagicResist)
{
	UE_MVVM_SET_PROPERTY_VALUE(ExtraMagicResist, NewExtraMagicResist);
}

float UBaseAttributeViewModel::GetEvasion() const
{
	return Evasion;
}

void UBaseAttributeViewModel::SetEvasion(float NewEvasion)
{
	UE_MVVM_SET_PROPERTY_VALUE(Evasion, NewEvasion);
}

float UBaseAttributeViewModel::GetAttackSpeed() const
{
	return AttackSpeed;
}

void UBaseAttributeViewModel::SetAttackSpeed(float NewAttackSpeed)
{
	UE_MVVM_SET_PROPERTY_VALUE(AttackSpeed, NewAttackSpeed);
}
