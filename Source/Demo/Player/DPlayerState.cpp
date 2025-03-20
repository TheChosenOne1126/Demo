// Fill out your copyright notice in the Description page of Project Settings.

#include "DPlayerState.h"
#include "DCharacter.h"
#include "Attribute/BaseAttributeSet.h"
#include "Component/DAbilitySystemComponent.h"
#include "DataAsset/AbilityDataAsset.h"
#include "DataAsset/PawnDataAsset.h"
#include "Global/DAssetManager.h"
#include "Global/GlobalTags.h"
#include "Global/Statics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "UI/BaseAttributeViewModel.h"

ADPlayerState::ADPlayerState()
{
	Asc = CreateDefaultSubobject<UDAbilitySystemComponent>(TEXT("DAbilitySystemComponent"));
	Asc->SetIsReplicated(true);
	Asc->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	CreateDefaultSubobject<UBaseAttributeSet>(TEXT("BaseAttributeSet"));

	BaseAttributeViewModel = CreateDefaultSubobject<UBaseAttributeViewModel>(TEXT("BaseAttributeViewModel"));

	SetNetUpdateFrequency(100.f);
}

void ADPlayerState::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (!IsValid(Asc))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid DAbilitySystemComponent"), __FUNCTION__));
		return;
	}

	Asc->RegisterAttributeValueChange(UBaseAttributeSet::GetHpAttribute(), this, &ThisClass::OnHpAttributeChange);
	Asc->RegisterAttributeValueChange(UBaseAttributeSet::GetMaxHpAttribute(), this, &ThisClass::OnMaxHpAttributeChange);
	Asc->RegisterAttributeValueChange(UBaseAttributeSet::GetHpRegenAttribute(), this, &ThisClass::OnHpRegenAttributeChange);
	Asc->RegisterAttributeValueChange(UBaseAttributeSet::GetMpAttribute(), this, &ThisClass::OnMpAttributeChange);
	Asc->RegisterAttributeValueChange(UBaseAttributeSet::GetMaxMpAttribute(), this, &ThisClass::OnMaxMpAttributeChange);
	Asc->RegisterAttributeValueChange(UBaseAttributeSet::GetMpRegenAttribute(), this, &ThisClass::OnMpRegenAttributeChange);
	Asc->RegisterAttributeValueChange(UBaseAttributeSet::GetLvAttribute(), this, &ThisClass::OnLvAttributeChange);
	Asc->RegisterAttributeValueChange(UBaseAttributeSet::GetDamageAttribute(), this, &ThisClass::OnDamageAttributeChange, false);
	Asc->RegisterAttributeValueChange(UBaseAttributeSet::GetExtraDamageAttribute(), this, &ThisClass::OnDamageAttributeChange, true);
	Asc->RegisterAttributeValueChange(UBaseAttributeSet::GetArmorAttribute(), this, &ThisClass::OnArmorAttributeChange, false);
	Asc->RegisterAttributeValueChange(UBaseAttributeSet::GetExtraArmorAttribute(), this, &ThisClass::OnArmorAttributeChange, true);
	Asc->RegisterAttributeValueChange(UBaseAttributeSet::GetMagicResistAttribute(), this, &ThisClass::OnMagicResistAttributeChange, false);
	Asc->RegisterAttributeValueChange(UBaseAttributeSet::GetExtraMagicResistAttribute(), this, &ThisClass::OnMagicResistAttributeChange, true);
}

void ADPlayerState::PostUnregisterAllComponents()
{
	if (!IsValid(Asc))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid DAbilitySystemComponent"), __FUNCTION__));
		return;
	}

	Asc->UnregisterAllAttributeValuesChange();

	if (HasAuthority())
	{
		Asc->ClearAllAbilities();

		Asc->RemoveLooseGameplayTag(GlobalTags::AbilitySystemInitTag);
	}

	Super::PostUnregisterAllComponents();
}

UAbilitySystemComponent* ADPlayerState::GetAbilitySystemComponent() const
{
	return Asc;
}

void ADPlayerState::OnPawnPossessed(ADCharacter* Character)
{
	if (!IsValid(Asc))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid Asc"), __FUNCTION__));
		return;
	}

	if (!Asc->HasMatchingGameplayTag(GlobalTags::AbilitySystemInitTag))
	{
		const UDAssetManager& AssetManager = UDAssetManager::GetRef();

		UPawnDataAsset* PawnDataAsset = AssetManager.GetPawnDataAsset();
		if (!IsValid(PawnDataAsset))
		{
			UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid PawnDataAsset"), __FUNCTION__));
			return;
		}

		const FPawnData* PawnDataPtr = PawnDataAsset->PawnDataArr.FindByPredicate(
			[Character](const FPawnData& PawnData) -> bool
			{
				return PawnData.PawnClass == Character->GetClass();
			});

		if (!PawnDataPtr)
		{
			UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: nullptr PawnDataPtr"), __FUNCTION__));
			return;
		}

		if (!IsValid(PawnDataPtr->AbilityDataAsset))
		{
			UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid AbilityDataAsset"), __FUNCTION__));
			return;
		}

		const TSubclassOf<UGameplayEffect> EffectClass = PawnDataPtr->AbilityDataAsset->InitialEffectClass;
		if (!IsValid(EffectClass))
		{
			UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid Initial Effect Class"), __FUNCTION__));
			return;
		}

		const FGameplayEffectSpecHandle EffectSpecHandle = Asc->MakeOutgoingSpec(EffectClass, 1.f, Asc->MakeEffectContext());
		Asc->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data, Asc->GetPredictionKeyForNewAction());

		for (FAbilityData& AbilityData : PawnDataPtr->AbilityDataAsset->AbilityDataArr)
		{
			if (!IsValid(AbilityData.AbilityClass))
			{
				UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid AbilityClass"), __FUNCTION__));
				continue;
			}

			FGameplayAbilitySpec AbilitySpec(AbilityData.AbilityClass, AbilityData.InitialAbilityLevel);
			AbilitySpec.GetDynamicSpecSourceTags().AppendTags(AbilityData.Tags);

			AbilitySpec.SetByCallerTagMagnitudes.Emplace(GlobalTags::SetByCallerAbilityMaxLevelTag, AbilityData.MaxAbilityLevel);
			Asc->GiveAbility(AbilitySpec);
		}

		Asc->AddLooseGameplayTag(GlobalTags::AbilitySystemInitTag);
	}
	else
	{
		const UBaseAttributeSet* BaseAttributeSet = Asc->GetSet<UBaseAttributeSet>();
		if (!IsValid(BaseAttributeSet))
		{
			UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid BaseAttributeSet"), __FUNCTION__));
			return;
		}

		Asc->ApplyModToAttribute(UBaseAttributeSet::GetHpAttribute(), EGameplayModOp::Override, BaseAttributeSet->GetMaxHp());
		Asc->ApplyModToAttribute(UBaseAttributeSet::GetMpAttribute(), EGameplayModOp::Override, BaseAttributeSet->GetMaxMp());
	}
}

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

void ADPlayerState::OnHpAttributeChange(const FOnAttributeChangeData& HpData)
{
	UStatics::Log(this, ELogType::Verbose, FString::Printf(TEXT("%hs: New Data = %f, Old Data = %f"),
		__FUNCTION__, HpData.NewValue, HpData.OldValue));

	if (!IsValid(BaseAttributeViewModel))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid AttributeViewModel"), __FUNCTION__));
		return;
	}

	BaseAttributeViewModel->SetHp(HpData.NewValue);

	if (HpData.NewValue <= 0.f)
	{
		Asc->AddLooseGameplayTag(GlobalTags::EventDeadTag);
	}
}

void ADPlayerState::OnMaxHpAttributeChange(const FOnAttributeChangeData& MaxHpData)
{
	UStatics::Log(this, ELogType::Verbose, FString::Printf(TEXT("%hs: New Data = %f, Old Data = %f"),
		__FUNCTION__, MaxHpData.NewValue, MaxHpData.OldValue));

	if (!IsValid(BaseAttributeViewModel))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid AttributeViewModel"), __FUNCTION__));
		return;
	}

	BaseAttributeViewModel->SetMaxHp(MaxHpData.NewValue);

	if (!HasAuthority())
	{
		return;
	}

	if (Asc->HasMatchingGameplayTag(GlobalTags::StateBlockHpRegenTag))
	{
		return;
	}

	const UBaseAttributeSet* BaseAttributeSet = Asc->GetSet<UBaseAttributeSet>();
	if (!IsValid(BaseAttributeSet))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid BaseAttributeSet"), __FUNCTION__));
		return;
	}

	float HpDelta;
	if (MaxHpData.OldValue > 0.f && BaseAttributeSet->GetHp() > 0.f)
	{
		const float MaxHpRatio = (MaxHpData.NewValue - MaxHpData.OldValue) / MaxHpData.OldValue;
		HpDelta = BaseAttributeSet->GetHp() * MaxHpRatio;
	}
	else
	{
		HpDelta = MaxHpData.NewValue;
	}
	
	Asc->ApplyModToAttribute(UBaseAttributeSet::GetHpAttribute(), EGameplayModOp::AddBase, HpDelta);
}

void ADPlayerState::OnHpRegenAttributeChange(const FOnAttributeChangeData& HpRegenData)
{
	UStatics::Log(this, ELogType::Verbose, FString::Printf(TEXT("%hs: New Data = %f, Old Data = %f"),
		__FUNCTION__, HpRegenData.NewValue, HpRegenData.OldValue));

	if (!IsValid(BaseAttributeViewModel))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid AttributeViewModel"), __FUNCTION__));
		return;
	}

	BaseAttributeViewModel->SetHpRegen(HpRegenData.NewValue);
}

void ADPlayerState::OnMpAttributeChange(const FOnAttributeChangeData& MpData)
{
	UStatics::Log(this, ELogType::Verbose, FString::Printf(TEXT("%hs: New Data = %f, Old Data = %f"),
		__FUNCTION__, MpData.NewValue, MpData.OldValue));

	if (!IsValid(BaseAttributeViewModel))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid AttributeViewModel"), __FUNCTION__));
		return;
	}

	BaseAttributeViewModel->SetMp(MpData.NewValue);
}

void ADPlayerState::OnMaxMpAttributeChange(const FOnAttributeChangeData& MaxMpData)
{
	UStatics::Log(this, ELogType::Verbose, FString::Printf(TEXT("%hs: New Data = %f, Old Data = %f"),
		__FUNCTION__, MaxMpData.NewValue, MaxMpData.OldValue));

	if (!IsValid(BaseAttributeViewModel))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid AttributeViewModel"), __FUNCTION__));
		return;
	}

	BaseAttributeViewModel->SetMaxHp(MaxMpData.NewValue);

	if (!HasAuthority())
	{
		return;
	}

	if (Asc->HasMatchingGameplayTag(GlobalTags::StateBlockMpRegenTag))
	{
		return;
	}

	const UBaseAttributeSet* BaseAttributeSet = Asc->GetSet<UBaseAttributeSet>();
	if (!IsValid(BaseAttributeSet))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid BaseAttributeSet"), __FUNCTION__));
		return;
	}
	
	float MpDelta;
	if (MaxMpData.OldValue > 0.f && BaseAttributeSet->GetHp() > 0.f)
	{
		const float MaxHpRatio = (MaxMpData.NewValue - MaxMpData.OldValue) / MaxMpData.OldValue;
		MpDelta = BaseAttributeSet->GetHp() * MaxHpRatio;
	}
	else
	{
		MpDelta = MaxMpData.NewValue;
	}
	
	Asc->ApplyModToAttribute(UBaseAttributeSet::GetHpAttribute(), EGameplayModOp::AddBase, MpDelta);
}

void ADPlayerState::OnMpRegenAttributeChange(const FOnAttributeChangeData& MpRegenData)
{
	UStatics::Log(this, ELogType::Verbose, FString::Printf(TEXT("%hs: New Data = %f, Old Data = %f"), __FUNCTION__, MpRegenData.NewValue, MpRegenData.OldValue));

	if (!IsValid(BaseAttributeViewModel))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid AttributeViewModel"), __FUNCTION__));
		return;
	}

	BaseAttributeViewModel->SetMpRegen(MpRegenData.NewValue);
}

void ADPlayerState::OnLvAttributeChange(const FOnAttributeChangeData& LvData)
{
	UStatics::Log(this, ELogType::Verbose, FString::Printf(TEXT("%hs: New Data = %f, Old Data = %f"),
		__FUNCTION__, LvData.NewValue, LvData.OldValue));

	if (!IsValid(BaseAttributeViewModel))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid AttributeViewModel"), __FUNCTION__));
		return;
	}

	BaseAttributeViewModel->SetLv(LvData.NewValue);
}

void ADPlayerState::OnDamageAttributeChange(const FOnAttributeChangeData& DamageData, bool bIsExtra)
{
	UStatics::Log(this, ELogType::Verbose, FString::Printf(TEXT("%hs: New Data = %f, Old Data = %f - %s"),
		__FUNCTION__, DamageData.NewValue, DamageData.OldValue, bIsExtra ? TEXT("Extra") : TEXT("Base")));

	if (!IsValid(BaseAttributeViewModel))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid AttributeViewModel - %s"),
			__FUNCTION__, bIsExtra ? TEXT("Extra") : TEXT("Base")));
		return;
	}

	if (bIsExtra)
	{
		BaseAttributeViewModel->SetExtraDamage(DamageData.NewValue);
	}
	else
	{
		BaseAttributeViewModel->SetDamage(DamageData.NewValue);
	}
}

void ADPlayerState::OnArmorAttributeChange(const FOnAttributeChangeData& ArmorData, bool bIsExtra)
{
	UStatics::Log(this, ELogType::Verbose, FString::Printf(TEXT("%hs: New Data = %f, Old Data = %f - %s"),
	__FUNCTION__, ArmorData.NewValue, ArmorData.OldValue, bIsExtra ? TEXT("Extra") : TEXT("Base")));

	if (!IsValid(BaseAttributeViewModel))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid AttributeViewModel - %s"),
			__FUNCTION__, bIsExtra ? TEXT("Extra") : TEXT("Base")));
		return;
	}

	if (bIsExtra)
	{
		BaseAttributeViewModel->SetExtraArmor(ArmorData.NewValue);
	}
	else
	{
		BaseAttributeViewModel->SetArmor(ArmorData.NewValue);
	}
}

void ADPlayerState::OnMagicResistAttributeChange(const FOnAttributeChangeData& MagicResistData, bool bIsExtra)
{
	UStatics::Log(this, ELogType::Verbose, FString::Printf(TEXT("%hs: New Data = %f, Old Data = %f - %s"),
		__FUNCTION__, MagicResistData.NewValue, MagicResistData.OldValue, bIsExtra ? TEXT("Extra") : TEXT("Base")));

	if (!IsValid(BaseAttributeViewModel))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid AttributeViewModel - %s"),
			__FUNCTION__, bIsExtra ? TEXT("Extra") : TEXT("Base")));
		return;
	}

	if (bIsExtra)
	{
		BaseAttributeViewModel->SetExtraMagicResist(MagicResistData.NewValue);
	}
	else
	{
		BaseAttributeViewModel->SetMagicResist(MagicResistData.NewValue);
	}
}

void ADPlayerState::OnAttackSpeedAttributeChange(const FOnAttributeChangeData& AttackSpeedData) const
{
	UStatics::Log(this, ELogType::Verbose, FString::Printf(TEXT("%hs: New Data = %f, Old Data = %f"),
		__FUNCTION__, AttackSpeedData.NewValue, AttackSpeedData.OldValue));

	if (!IsValid(BaseAttributeViewModel))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid AttributeViewModel"), __FUNCTION__));
		return;
	}

	BaseAttributeViewModel->SetAttackSpeed(AttackSpeedData.NewValue);
}