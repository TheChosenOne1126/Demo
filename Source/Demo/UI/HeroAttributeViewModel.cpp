// Fill out your copyright notice in the Description page of Project Settings.

#include "HeroAttributeViewModel.h"

float UHeroAttributeViewModel::GetXp() const
{
	return Xp;
}

void UHeroAttributeViewModel::SetXp(float NewXp)
{
	UE_MVVM_SET_PROPERTY_VALUE(Xp, NewXp);
}

float UHeroAttributeViewModel::GetStrength() const
{
	return Strength;
}

void UHeroAttributeViewModel::SetStrength(float NewStrength)
{
	UE_MVVM_SET_PROPERTY_VALUE(Strength, NewStrength);
}

float UHeroAttributeViewModel::GetExtraStrength() const
{
	return ExtraStrength;
}

void UHeroAttributeViewModel::SetExtraStrength(float NewExtraStrength)
{
	UE_MVVM_SET_PROPERTY_VALUE(ExtraStrength, NewExtraStrength);
}

float UHeroAttributeViewModel::GetIntelligence() const
{
	return Intelligence;
}

void UHeroAttributeViewModel::SetIntelligence(float NewIntelligence)
{
	UE_MVVM_SET_PROPERTY_VALUE(Intelligence, NewIntelligence);
}

float UHeroAttributeViewModel::GetExtraIntelligence() const
{
	return ExtraIntelligence;
}

void UHeroAttributeViewModel::SetExtraIntelligence(float NewExtraIntelligence)
{
	UE_MVVM_SET_PROPERTY_VALUE(ExtraIntelligence, NewExtraIntelligence);
}

float UHeroAttributeViewModel::GetAgility() const
{
	return Agility;
}

void UHeroAttributeViewModel::SetAgility(float NewAgility)
{
	UE_MVVM_SET_PROPERTY_VALUE(Agility, NewAgility);
}

float UHeroAttributeViewModel::GetExtraAgility() const
{
	return ExtraAgility;
}

void UHeroAttributeViewModel::SetExtraAgility(float NewExtraAgility)
{
	UE_MVVM_SET_PROPERTY_VALUE(ExtraAgility, NewExtraAgility);
}