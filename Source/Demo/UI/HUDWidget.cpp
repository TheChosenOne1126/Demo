// Fill out your copyright notice in the Description page of Project Settings.

#include "HUDWidget.h"
#include "AbilitySlotWidget.h"
#include "Components/HorizontalBox.h"
#include "Global/GlobalTags.h"
#include "Global/MessageSubsystem.h"
#include "Global/Statics.h"

void UHUDWidget::InitAbilitySlot(const TArray<FAbilitySlotData>& AbilitySlotDataArr)
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

	for (const FAbilitySlotData& AbilitySlotData : AbilitySlotDataArr)
	{
		MessageSubsystem->BroadcastMessage(GlobalTags::AbilitySlotInitTag, AbilitySlotData);
	}
}
