// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySlotWidget.h"
#include "AbilitySystemComponent.h"
#include "CommonButtonBase.h"
#include "CommonNumericTextBlock.h"
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

FGameplayAbilitySpec* UAbilitySlotWidget::GetAbilitySpecByHandle() const
{
	const AHeroPlayerState* PlayerState = GetOwningPlayerState<AHeroPlayerState>();
	if (!IsValid(PlayerState))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid PlayerState"), __FUNCTION__));
		return nullptr;
	}

	const UAbilitySystemComponent* Asc = PlayerState->GetAbilitySystemComponent();
	if (!IsValid(Asc))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid AbilitySystemComponent"), __FUNCTION__));
		return nullptr;
	}

	return Asc->FindAbilitySpecFromHandle(AbilitySpecHandle);
}

void UAbilitySlotWidget::OnAbilitySlotInitialized(const FAbilitySlotData& AbilitySlotData)
{
	if (!AbilitySlotData.SlotTags.HasTagExact(AbilitySlotTag))
	{
		return;
	}
	
	if (!IsValid(AbilitySlotData.AbilityTexture))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid AbilityTexture"), __FUNCTION__));
		return;
	}

	AbilitySpecHandle = AbilitySlotData.AbilitySpecHandle;

	AbilityImage->SetBrushFromTexture(AbilitySlotData.AbilityTexture);
	AbilityLevelText->SetCurrentValue(AbilitySlotData.AbilityLevel);
}

void UAbilitySlotWidget::OnAbilityLevelUpdated(const FGameplayAbilitySpec& AbilitySpec)
{
	if (AbilitySpecHandle != AbilitySpec.Handle)
	{
		return;
	}
	
	AbilityLevelText->SetCurrentValue(AbilitySpec.Level);
}

void UAbilitySlotWidget::OnAbilityLevelUpdateButtonClicked()
{
	AHeroPlayerState* PlayerState = GetOwningPlayerState<AHeroPlayerState>();
	if (!IsValid(PlayerState))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid PlayerState"), __FUNCTION__));
		return;
	}

	PlayerState->ServerUpdateAbilityLevel(AbilitySpecHandle);
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
