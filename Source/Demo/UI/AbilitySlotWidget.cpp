// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySlotWidget.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "CommonButtonBase.h"
#include "CommonNumericTextBlock.h"
#include "Ability/DGameplayAbility.h"
#include "Components/Image.h"
#include "Global/GlobalTags.h"
#include "Global/MessageSubsystem.h"
#include "Global/Statics.h"
#include "Player/HeroPlayerState.h"

void UAbilitySlotWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UMessageSubsystem* MessageSubsystem = UMessageSubsystem::Get(this);
	if (!IsValid(MessageSubsystem))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid Message Subsystem"), __FUNCTION__));
		return;
	}
	
	MessageHandleMap.Emplace(GlobalTags::AbilitySlotInitTag) = MessageSubsystem->RegisterMessage(
		GlobalTags::AbilitySlotInitTag, this, &ThisClass::OnAbilitySlotInitialized);
	MessageHandleMap.Emplace(GlobalTags::AbilityUpdateLevelTag) = MessageSubsystem->RegisterMessage(
		GlobalTags::AbilityUpdateLevelTag, this, &ThisClass::OnAbilityLevelUpdated);
	MessageHandleMap.Emplace(GlobalTags::AttributeBaseAbilityPointChangedTag) = MessageSubsystem->RegisterMessage(
		GlobalTags::AttributeBaseAbilityPointChangedTag, this, &ThisClass::OnAttributeBaseSpChanged);
	MessageHandleMap.Emplace(GlobalTags::AttributeUltimateAbilityPointChangedTag) = MessageSubsystem->RegisterMessage(
		GlobalTags::AttributeUltimateAbilityPointChangedTag, this, &ThisClass::OnAttributeUltimateSpChanged);

	if (IsValid(AbilityLevelUpdateButton))
	{
		AbilityUpdateLevelClickDelegateHandle = AbilityLevelUpdateButton->OnClicked().AddUObject(
			this, &UAbilitySlotWidget::OnAbilityLevelUpdateButtonClicked);
	}
}

void UAbilitySlotWidget::NativeDestruct()
{
	UMessageSubsystem* MessageSubsystem = UMessageSubsystem::Get(this);
	if (!IsValid(MessageSubsystem))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid Message Subsystem"), __FUNCTION__));
		return;
	}

	for (auto& [MessageTag, MessageId] : MessageHandleMap)
	{
		MessageSubsystem->UnregisterMessage(MessageTag, MessageId);
	}

	if (IsValid(AbilityLevelUpdateButton))
	{
		AbilityLevelUpdateButton->OnClicked().Remove(AbilityUpdateLevelClickDelegateHandle);
	}
	
	Super::NativeDestruct();
}

void UAbilitySlotWidget::OnAbilitySlotInitialized(const FAbilityData& AbilityData)
{
	if (!AbilityData.Tags.HasTagExact(AbilitySlotTag))
	{
		return;
	}

	if (!IsValid(AbilityData.AbilityTexture))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid AbilityTexture"), __FUNCTION__));
		return;
	}

	AbilityImage->SetBrushFromTexture(AbilityData.AbilityTexture);
	AbilityLevelText->SetCurrentValue(AbilityData.InitialAbilityLevel);
}

void UAbilitySlotWidget::OnAbilityLevelUpdated(const FGameplayAbilitySpecHandle& SpecHandle)
{
	// if (AbilitySpecHandle != SpecHandle)
	// {
	// 	return;
	// }
	//
	// const FGameplayAbilitySpec* AbilitySpec = GetAbilitySpecByHandle();
	// if (!AbilitySpec)
	// {
	// 	UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid AbilitySpec"), __FUNCTION__));
	// 	return;
	// }
	//
	// AbilityLevelText->SetCurrentValue(AbilitySpec->Level);
}

void UAbilitySlotWidget::OnAbilityLevelUpdateButtonClicked()
{
	AHeroPlayerState* PlayerState = GetOwningPlayerState<AHeroPlayerState>();
	if (!IsValid(PlayerState))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid PlayerState"), __FUNCTION__));
		return;
	}

	// PlayerState->ServerUpdateAbilityLevel(AbilitySpecHandle);
}

void UAbilitySlotWidget::OnAttributeAbilityPointChanged(float AbilityPointValue, const FGameplayAbilitySpec* AbilitySpec)
{
	if (AbilityPointValue > 0.f)
	{
		const float* MaxAbilityLevelPtr = AbilitySpec->SetByCallerTagMagnitudes.Find(GlobalTags::SetByCallerAbilityMaxLevelTag);
		if (!MaxAbilityLevelPtr)
		{
			UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid MaxLevel"), __FUNCTION__));
			return;
		}

		if (AbilitySpec->Level >= *MaxAbilityLevelPtr)
		{
			return;
		}

		AbilityLevelUpdateButton->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		AbilityLevelUpdateButton->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UAbilitySlotWidget::OnAttributeBaseSpChanged(const FMessageFloatStructType& MessageStruct)
{
	const FGameplayAbilitySpec* AbilitySpec = GetAbilitySpecByHandle();
	if (!AbilitySpec)
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid AbilitySpec"), __FUNCTION__));
		return;
	}
	
	if (AbilitySpec->GetDynamicSpecSourceTags().HasTagExact(GlobalTags::AbilityUltimateTag))
	{
		return;
	}

	OnAttributeAbilityPointChanged(MessageStruct.Value, AbilitySpec);
}

void UAbilitySlotWidget::OnAttributeUltimateSpChanged(const FMessageFloatStructType& MessageStruct)
{
	const FGameplayAbilitySpec* AbilitySpec = GetAbilitySpecByHandle();
	if (!AbilitySpec)
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid AbilitySpec"), __FUNCTION__));
		return;
	}
	
	if (!AbilitySpec->GetDynamicSpecSourceTags().HasTagExact(GlobalTags::AbilityUltimateTag))
	{
		return;
	}

	OnAttributeAbilityPointChanged(MessageStruct.Value, AbilitySpec);
}
