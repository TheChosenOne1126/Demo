// Fill out your copyright notice in the Description page of Project Settings.

#include "HUDWidget.h"
#include "AbilitySlotWidget.h"
#include "Components/HorizontalBox.h"
#include "Global/Statics.h"

void UHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (!IsValid(AbilitySlotBox))
	{
		UStatics::Log(this, ELogType::Error, FString::Printf(TEXT("%hs: invalid AbilitySlotBox"), __FUNCTION__));
		return;
	}

	for (UWidget* Child : AbilitySlotBox->GetAllChildren())
	{
		UAbilitySlotWidget* AbilitySlotWidget = Cast<UAbilitySlotWidget>(Child);
		if (IsValid(AbilitySlotWidget))
		{
			AbilitySlotWidgets.Emplace(AbilitySlotWidget);
		}
	}
}
