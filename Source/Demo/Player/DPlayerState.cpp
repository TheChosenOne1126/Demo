// Fill out your copyright notice in the Description page of Project Settings.

#include "DPlayerState.h"
#include "Attribute/BaseAttributeSet.h"
#include "Component/DAbilitySystemComponent.h"
#include "DataAsset/AbilityDataAsset.h"
#include "Global/GlobalTags.h"
#include "Kismet/KismetSystemLibrary.h"

ADPlayerState::ADPlayerState()
{
	Asc = CreateDefaultSubobject<UDAbilitySystemComponent>(TEXT("DAbilitySystemComponent"));
	Asc->SetIsReplicated(true);
	Asc->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	CreateDefaultSubobject<UBaseAttributeSet>(TEXT("BaseAttributeSet"));

	SetNetUpdateFrequency(100.f);
}

UAbilitySystemComponent* ADPlayerState::GetAbilitySystemComponent() const
{
	return Asc;
}

FString ADPlayerState::GetModuleName_Implementation() const
{
	return TEXT("Player.PlayerState");
}

void ADPlayerState::NetMulticastShowDamageNumber_Implementation(float DamageValue, FGameplayTagContainer DamageTags)
{
	if (UKismetSystemLibrary::IsDedicatedServer(this))
	{
		return;
	}

	if (DamageTags.HasTagExact(GlobalTags::GE_ATTACK_MISS))
	{
		// TODO: Display Missing UI
	}
	else
	{
		// TODO: Display Damage Number UI
	}
}

// void ADPlayerState::OnHpAttributeChange(const FOnAttributeChangeData& HpData)
// {
// 	UStatics::Log(this, ELogType::Verbose, FString::Printf(TEXT("%hs: New Data = %f, Old Data = %f"),
// 		__FUNCTION__, HpData.NewValue, HpData.OldValue));
//
// 	if (HpData.NewValue <= 0.f)
// 	{
// 		Asc->AddLooseGameplayTag(GlobalTags::EventDeadTag);
// 	}
// }

// void ADPlayerState::OnMaxHpAttributeChange(const FOnAttributeChangeData& MaxHpData)
// {
// 	UStatics::Log(this, ELogType::Verbose, FString::Printf(TEXT("%hs: New Data = %f, Old Data = %f"),
// 		__FUNCTION__, MaxHpData.NewValue, MaxHpData.OldValue));
// 	
// 	if (!HasAuthority())
// 	{
// 		return;
// 	}
//
// 	if (Asc->HasMatchingGameplayTag(GlobalTags::StateBlockHpRegenTag))
// 	{
// 		return;
// 	}
//
// 	const UBaseAttributeSet* BaseAttributeSet = Asc->GetSet<UBaseAttributeSet>();
// 	if (!IsValid(BaseAttributeSet))
// 	{
// 		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid BaseAttributeSet"), __FUNCTION__));
// 		return;
// 	}
//
// 	float HpDelta;
// 	if (MaxHpData.OldValue > 0.f && BaseAttributeSet->GetHp() > 0.f)
// 	{
// 		const float MaxHpRatio = (MaxHpData.NewValue - MaxHpData.OldValue) / MaxHpData.OldValue;
// 		HpDelta = BaseAttributeSet->GetHp() * MaxHpRatio;
// 	}
// 	else
// 	{
// 		HpDelta = MaxHpData.NewValue;
// 	}
// 	
// 	Asc->ApplyModToAttribute(UBaseAttributeSet::GetHpAttribute(), EGameplayModOp::AddBase, HpDelta);
// }

// void ADPlayerState::OnMaxMpAttributeChange(const FOnAttributeChangeData& MaxMpData)
// {
// 	UStatics::Log(this, ELogType::Verbose, FString::Printf(TEXT("%hs: New Data = %f, Old Data = %f"),
// 		__FUNCTION__, MaxMpData.NewValue, MaxMpData.OldValue));
// 	
// 	if (!HasAuthority())
// 	{
// 		return;
// 	}
//
// 	if (Asc->HasMatchingGameplayTag(GlobalTags::StateBlockMpRegenTag))
// 	{
// 		return;
// 	}
//
// 	const UBaseAttributeSet* BaseAttributeSet = Asc->GetSet<UBaseAttributeSet>();
// 	if (!IsValid(BaseAttributeSet))
// 	{
// 		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid BaseAttributeSet"), __FUNCTION__));
// 		return;
// 	}
// 	
// 	float MpDelta;
// 	if (MaxMpData.OldValue > 0.f && BaseAttributeSet->GetHp() > 0.f)
// 	{
// 		const float MaxHpRatio = (MaxMpData.NewValue - MaxMpData.OldValue) / MaxMpData.OldValue;
// 		MpDelta = BaseAttributeSet->GetHp() * MaxHpRatio;
// 	}
// 	else
// 	{
// 		MpDelta = MaxMpData.NewValue;
// 	}
// 	
// 	Asc->ApplyModToAttribute(UBaseAttributeSet::GetHpAttribute(), EGameplayModOp::AddBase, MpDelta);
// }