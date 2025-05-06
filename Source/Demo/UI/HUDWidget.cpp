// Fill out your copyright notice in the Description page of Project Settings.

#include "HUDWidget.h"
#include "AbilitySlotWidget.h"
#include "Components/HorizontalBox.h"
#include "Global/GlobalTags.h"
#include "Global/Statics.h"

void UHUDWidget::InitAbilitySlot(const TArray<FAbilityData>& SlotAbilityDataArr)
{
	if (!IsValid(AbilitySlotBox))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid AbilitySlotBox"), __FUNCTION__));
		return;
	}

	UMessageSubsystem* MessageSubsystem = UMessageSubsystem::Get(this);
	if (!IsValid(MessageSubsystem))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid MessageSubsystem"), __FUNCTION__));
		return;
	}

	for (const FAbilityData& AbilityData : SlotAbilityDataArr)
	{
		MessageSubsystem->BroadcastMessage(GlobalTags::AbilitySlotInitTag, AbilityData);
	}
}
