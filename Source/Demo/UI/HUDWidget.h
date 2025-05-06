// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DCommonUserWidget.h"
#include "Interface/AbilitySlotInterface.h"
#include "HUDWidget.generated.h"

class UAbilitySlotWidget;
class UHorizontalBox;

UCLASS(Abstract)
class DEMO_API UHUDWidget : public UDCommonUserWidget
{
	GENERATED_BODY()

public:
	void InitAbilitySlot(const TArray<FAbilitySlotData>& AbilitySlotDataArr);

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UHorizontalBox> AbilitySlotBox;
};
