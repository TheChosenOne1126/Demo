// Fill out your copyright notice in the Description page of Project Settings.

#include "HeroPlayerState.h"
#include "Attribute/HeroAttributeSet.h"
#include "Global/GlobalTags.h"
#include "Global/MessageStructTypes.h"
#include "Global/MessageSubsystem.h"
#include "Global/Statics.h"
#include "UI/HeroAttributeViewModel.h"

AHeroPlayerState::AHeroPlayerState()
{
	CreateDefaultSubobject<UHeroAttributeSet>(TEXT("HeroAttributeSet"));
	
	HeroAttributeViewModel = CreateDefaultSubobject<UHeroAttributeViewModel>("HeroAttributeViewModel");
}

void AHeroPlayerState::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (!IsValid(Asc))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid AbilitySystemComponent"), __FUNCTION__));
		return;
	}

	Asc->RegisterAttributeValueChange(UHeroAttributeSet::GetXpAttribute(), this, &ThisClass::OnXpAttributeChange);
	Asc->RegisterAttributeValueChange(UHeroAttributeSet::GetStrengthAttribute(), this, &ThisClass::OnStrengthAttributeChange, false);
	Asc->RegisterAttributeValueChange(UHeroAttributeSet::GetExtraStrengthAttribute(), this, &ThisClass::OnStrengthAttributeChange, true);
	Asc->RegisterAttributeValueChange(UHeroAttributeSet::GetIntelligenceAttribute(), this, &ThisClass::OnIntelligenceAttributeChange, false);
	Asc->RegisterAttributeValueChange(UHeroAttributeSet::GetExtraIntelligenceAttribute(), this, &ThisClass::OnIntelligenceAttributeChange, true);
	Asc->RegisterAttributeValueChange(UHeroAttributeSet::GetAgilityAttribute(), this, &ThisClass::OnAgilityAttributeChange, false);
	Asc->RegisterAttributeValueChange(UHeroAttributeSet::GetExtraAgilityAttribute(), this, &ThisClass::OnAgilityAttributeChange, true);
	Asc->RegisterAttributeValueChange(UHeroAttributeSet::GetBaseSpAttribute(), this, &ThisClass::OnSpAttributeChange, false);
	Asc->RegisterAttributeValueChange(UHeroAttributeSet::GetUltimateSpAttribute(), this, &ThisClass::OnSpAttributeChange, true);
}

void AHeroPlayerState::OnPawnPossessed(ADCharacter* Character)
{
	if (!IsValid(Asc))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid AbilitySystemComponent"), __FUNCTION__));
		return;
	}

	if (Asc->HasMatchingGameplayTag(GlobalTags::AbilitySystemInitTag))
	{
		return;
	}
	
	Super::OnPawnPossessed(Character);

	float MaxAbilityPoint = 0.f;
	for (FGameplayAbilitySpec& AbilitySpec : Asc->GetActivatableAbilities())
	{
		if (AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(GlobalTags::AbilityActivationAutoTag))
		{
			continue;
		}

		const float* MaxLevelPtr = AbilitySpec.SetByCallerTagMagnitudes.Find(GlobalTags::SetByCallerAbilityMaxLevelTag);
		if (!MaxLevelPtr)
		{
			UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid MaxLevelPtr"), __FUNCTION__));
			continue;
		}

		MaxAbilityPoint += *MaxLevelPtr - AbilitySpec.Level;
	}

	Asc->ApplyModToAttribute(UHeroAttributeSet::GetMaxSpAttribute(), EGameplayModOp::Override, MaxAbilityPoint);
}

void AHeroPlayerState::ServerUpdateAbilityLevel_Implementation(FGameplayAbilitySpecHandle AbilitySpecHandle)
{
	if (!AbilitySpecHandle.IsValid())
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid AbilitySpecHandle"), __FUNCTION__));
		return;
	}

	if (!IsValid(Asc))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid Asc"), __FUNCTION__));
		return;
	}

	FGameplayAbilitySpec* AbilitySpec = Asc->FindAbilitySpecFromHandle(AbilitySpecHandle);
	if (!AbilitySpec)
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid AbilitySpec"), __FUNCTION__));
		return;
	}
	
	AbilitySpec->Level++;

	if (AbilitySpec->GetDynamicSpecSourceTags().HasTagExact(GlobalTags::AbilityUltimateTag))
	{
		Asc->ApplyModToAttribute(UHeroAttributeSet::GetUltimateSpAttribute(), EGameplayModOp::AddBase, -1.f);
	}
	else
	{
		Asc->ApplyModToAttribute(UHeroAttributeSet::GetBaseSpAttribute(), EGameplayModOp::AddBase, -1.f);
	}

	Asc->ApplyModToAttribute(UHeroAttributeSet::GetMaxSpAttribute(), EGameplayModOp::AddBase, -1.f);

	NetMulticastUpdateAbilityLevel(AbilitySpecHandle);
}

bool AHeroPlayerState::ServerUpdateAbilityLevel_Validate(FGameplayAbilitySpecHandle AbilitySpecHandle)
{
	if (!AbilitySpecHandle.IsValid())
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid AbilitySpecHandle"), __FUNCTION__));
		return false;
	}

	if (!IsValid(Asc))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid Asc"), __FUNCTION__));
		return false;
	}

	FGameplayAbilitySpec* AbilitySpec = Asc->FindAbilitySpecFromHandle(AbilitySpecHandle);
	if (!AbilitySpec)
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid AbilitySpec"), __FUNCTION__));
		return false;;
	}

	const UHeroAttributeSet* HeroAttributeSet = Asc->GetSet<UHeroAttributeSet>();
	if (!IsValid(HeroAttributeSet))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid HeroAttributeSet"), __FUNCTION__));
		return false;
	}
	
	const float* AbilityMaxLevelPtr = AbilitySpec->SetByCallerTagMagnitudes.Find(GlobalTags::SetByCallerAbilityMaxLevelTag);
	if (!AbilityMaxLevelPtr)
	{
		return false;
	}

	if (AbilitySpec->Level < *AbilityMaxLevelPtr)
	{
		if (AbilitySpec->GetDynamicSpecSourceTags().HasTagExact(GlobalTags::AbilityUltimateTag))
		{
			return HeroAttributeSet->GetUltimateSp() > 0.f;
		}

		return HeroAttributeSet->GetBaseSp() > 0.f;
	}

	return false;
}

void AHeroPlayerState::NetMulticastUpdateAbilityLevel_Implementation(FGameplayAbilitySpecHandle AbilitySpecHandle)
{
	if (!AbilitySpecHandle.IsValid())
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid AbilitySpecHandle"), __FUNCTION__));
		return;
	}

	UMessageSubsystem* MessageSubsystem = UMessageSubsystem::Get(this);
	if (!IsValid(MessageSubsystem))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid MessageSubsystem"), __FUNCTION__));
		return;
	}

	MessageSubsystem->BroadcastMessage(GlobalTags::AbilityUpdateLevelTag, AbilitySpecHandle);
}

void AHeroPlayerState::OnXpAttributeChange(const FOnAttributeChangeData& XpData)
{
	UStatics::Log(this, ELogType::Verbose, FString::Printf(TEXT("%hs: New Data = %f, Old Data = %f"), __FUNCTION__, XpData.NewValue, XpData.OldValue));

	if (!IsValid(HeroAttributeViewModel))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid AttributeViewModel"), __FUNCTION__));
		return;
	}

	HeroAttributeViewModel->SetXp(XpData.NewValue);
}

void AHeroPlayerState::OnStrengthAttributeChange(const FOnAttributeChangeData& StrengthData, bool bIsExtra)
{
	UStatics::Log(this, ELogType::Verbose, FString::Printf(TEXT("%hs: New Data = %f, Old Data = %f - %s"),
		__FUNCTION__, StrengthData.NewValue, StrengthData.OldValue, bIsExtra ? TEXT("Extra") : TEXT("Base")));

	if (!IsValid(HeroAttributeViewModel))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid AttributeViewModel - %s"),
			__FUNCTION__, bIsExtra ? TEXT("Extra") : TEXT("Base")));
		return;
	}

	if (bIsExtra)
	{
		HeroAttributeViewModel->SetExtraStrength(StrengthData.NewValue);
	}
	else
	{
		HeroAttributeViewModel->SetStrength(StrengthData.NewValue);
	}
}

void AHeroPlayerState::OnIntelligenceAttributeChange(const FOnAttributeChangeData& IntelligenceData, bool bIsExtra)
{
	UStatics::Log(this, ELogType::Verbose, FString::Printf(TEXT("%hs: New Data = %f, Old Data = %f - %s"),
		__FUNCTION__, IntelligenceData.NewValue, IntelligenceData.OldValue, bIsExtra ? TEXT("Extra") : TEXT("Base")));

	if (!IsValid(HeroAttributeViewModel))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid AttributeViewModel - %s"),
			__FUNCTION__, bIsExtra ? TEXT("Extra") : TEXT("Base")));
		return;
	}

	if (bIsExtra)
	{
		HeroAttributeViewModel->SetExtraIntelligence(IntelligenceData.NewValue);
	}
	else
	{
		HeroAttributeViewModel->SetIntelligence(IntelligenceData.NewValue);
	}
}

void AHeroPlayerState::OnAgilityAttributeChange(const FOnAttributeChangeData& AgilityData, bool bIsExtra)
{
	UStatics::Log(this, ELogType::Verbose, FString::Printf(TEXT("%hs: New Data = %f, Old Data = %f - %s"),
		__FUNCTION__, AgilityData.NewValue, AgilityData.OldValue, bIsExtra ? TEXT("Extra") : TEXT("Base")));

	if (!IsValid(HeroAttributeViewModel))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid AttributeViewModel - %s"),
			__FUNCTION__, bIsExtra ? TEXT("Extra") : TEXT("Base")));
		return;
	}

	if (bIsExtra)
	{
		HeroAttributeViewModel->SetExtraAgility(AgilityData.NewValue);
	}
	else
	{
		HeroAttributeViewModel->SetAgility(AgilityData.NewValue);
	}
}

void AHeroPlayerState::OnSpAttributeChange(const FOnAttributeChangeData& SpData, bool bIsUltimate)
{
	if (HasAuthority())
	{
		return;
	}
	
	UStatics::Log(this, ELogType::Verbose, FString::Printf(TEXT("%hs: New Data = %f, Old Data = %f - %s"),
		__FUNCTION__, SpData.NewValue, SpData.OldValue, bIsUltimate ? TEXT("Ultimate") : TEXT("Base")));

	UMessageSubsystem* MessageSubsystem = UMessageSubsystem::Get(this);
	if (!IsValid(MessageSubsystem))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid Message Subsystem"), __FUNCTION__));
		return;
	}

	FMessageFloatStructType FloatStruct;
	FloatStruct.Value = SpData.NewValue;
	if (bIsUltimate)
	{
		MessageSubsystem->BroadcastMessage(GlobalTags::AttributeUltimateAbilityPointChangedTag, FloatStruct);
	}
	else
	{
		MessageSubsystem->BroadcastMessage(GlobalTags::AttributeBaseAbilityPointChangedTag, FloatStruct);
	}
}
