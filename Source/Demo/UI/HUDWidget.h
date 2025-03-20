// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DCommonUserWidget.h"
#include "HUDWidget.generated.h"

class UAbilitySlotWidget;
class UHorizontalBox;

UCLASS(Abstract)
class DEMO_API UHUDWidget : public UDCommonUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UHorizontalBox> AbilitySlotBox;

	UPROPERTY()
	TArray<TObjectPtr<UAbilitySlotWidget>> AbilitySlotWidgets;
};
