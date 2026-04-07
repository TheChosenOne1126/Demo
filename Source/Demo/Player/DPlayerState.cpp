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

// void ADPlayerState::InitAbilitySystem(ADCharacter* Character)
// {
// 	if (!IsValid(Character))
// 	{
// 		return;
// 	}
//
// 	if (!Character->GetPawnTag().IsValid())
// 	{
// 		return;
// 	}
// 	
// 	if (!IsValid(Asc))
// 	{
// 		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid Asc"), __FUNCTION__));
// 		return;
// 	}
//
// 	if (!Asc->HasMatchingGameplayTag(GlobalTags::AbilityGivenTag))
// 	{
// 		const UDAssetManager& AssetManager = UDAssetManager::Get();
//
// 		UPawnDataAsset* PawnDataAsset = AssetManager.GetPawnDataAsset();
// 		if (!IsValid(PawnDataAsset))
// 		{
// 			UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid PawnDataAsset"), __FUNCTION__));
// 			return;
// 		}
//
// 		if (!PawnDataAsset->PawnDataMap.Contains(Character->GetPawnTag()))
// 		{
// 			UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: PawnDataMap no contains Tag:[%s]"),
// 				__FUNCTION__, *Character->GetPawnTag().ToString()));
// 			return;
// 		}
// 		
// 		UAbilityDataAsset* AbilityDataAsset = PawnDataAsset->PawnDataMap[Character->GetPawnTag()].AbilityDataAsset;
// 		if (!IsValid(AbilityDataAsset))
// 		{
// 			UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid AbilityDataAsset"), __FUNCTION__));
// 			return;
// 		}
//
// 		const TSubclassOf<UGameplayEffect> EffectClass = AbilityDataAsset->InitialEffectClass;
// 		if (!IsValid(EffectClass))
// 		{
// 			UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid Initial Effect Class"), __FUNCTION__));
// 			return;
// 		}
//
// 		const FGameplayEffectSpecHandle EffectSpecHandle = Asc->MakeOutgoingSpec(EffectClass, 1.f, Asc->MakeEffectContext());
// 		Asc->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data, Asc->GetPredictionKeyForNewAction());
//
// 		for (const auto& [AbilityClass, AbilityData] : AbilityDataAsset->AbilityDataMap)
// 		{
// 			if (!IsValid(AbilityClass))
// 			{
// 				UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid AbilityClass"), __FUNCTION__));
// 				continue;
// 			}
//
// 			if (AbilityClass->HasAnyClassFlags(CLASS_Abstract))
// 			{
// 				UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: Abstract AbilityClass"), __FUNCTION__));
// 				continue;
// 			}
//
// 			FGameplayAbilitySpec AbilitySpec(AbilityClass, AbilityData.InitialAbilityLevel);
// 			AbilitySpec.GetDynamicSpecSourceTags().AppendTags(AbilityData.Tags);
//
// 			AbilitySpec.SetByCallerTagMagnitudes.Emplace(GlobalTags::SetByCallerAbilityMaxLevelTag, AbilityData.MaxAbilityLevel);
// 			const FGameplayAbilitySpecHandle AbilitySpecHandle = Asc->GiveAbility(AbilitySpec);
// 		}
//
// 		Asc->AddLooseGameplayTag(GlobalTags::AbilityGivenTag);
// 	}
// 	else
// 	{
// 		const UBaseAttributeSet* BaseAttributeSet = Asc->GetSet<UBaseAttributeSet>();
// 		if (!IsValid(BaseAttributeSet))
// 		{
// 			UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid BaseAttributeSet"), __FUNCTION__));
// 			return;
// 		}
//
// 		Asc->ApplyModToAttribute(UBaseAttributeSet::GetHpAttribute(), EGameplayModOp::Override, BaseAttributeSet->GetMaxHp());
// 		Asc->ApplyModToAttribute(UBaseAttributeSet::GetMpAttribute(), EGameplayModOp::Override, BaseAttributeSet->GetMaxMp());
// 	}
// }

void ADPlayerState::NetMulticastShowDamageNumber_Implementation(float DamageValue, FGameplayTagContainer DamageTags)
{
	if (UKismetSystemLibrary::IsDedicatedServer(this))
	{
		return;
	}

	if (DamageTags.HasTagExact(GlobalTags::EffectDamageMissTag))
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