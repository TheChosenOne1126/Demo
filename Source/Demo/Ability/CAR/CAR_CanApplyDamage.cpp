// Fill out your copyright notice in the Description page of Project Settings.

#include "CAR_CanApplyDamage.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "Attribute/BaseAttributeSet.h"
#include "Global/GlobalTags.h"
#include "Global/Statics.h"
#include "Player/DPlayerState.h"

bool UCAR_CanApplyDamage::CanApplyGameplayEffect_Implementation(
	const UGameplayEffect* GameplayEffect,
	const FGameplayEffectSpec& Spec,
	UAbilitySystemComponent* Asc) const
{
	FGameplayTagContainer GrantedTags;
	Spec.GetAllGrantedTags(GrantedTags);
	if (!GrantedTags.HasTagExact(GlobalTags::EffectDamageNormalAttackTag))
	{
		return true;
	}

	if (!IsValid(Asc))
	{
		UStatics::Log(GameplayEffect, ELogType::Error, FString::Printf(TEXT("%hs: invalid AbilitySystemComponent"), __FUNCTION__));
		return false;
	}

	const float Evasion = Asc->GetNumericAttribute(UBaseAttributeSet::GetEvasionAttribute());
	if (FMath::RandRange(0.f, 1.f) > Evasion)
	{
		ADPlayerState* Ps = Cast<ADPlayerState>(Asc->GetOwnerActor());
		if (!IsValid(Ps))
		{
			Ps->NetMulticastShowDamageNumber(0.f, FGameplayTagContainer(GlobalTags::EffectDamageMissTag));
		}
		else
		{
			UStatics::Log(GameplayEffect, ELogType::Error, FString::Printf(TEXT("%hs: invalid ADPlayerState"), __FUNCTION__));
		}
		return true;
	}

	return false;
}
