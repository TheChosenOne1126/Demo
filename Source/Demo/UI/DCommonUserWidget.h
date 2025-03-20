// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "DCommonUserWidget.generated.h"

class UBaseAttributeViewModel;

UCLASS(Abstract)
class DEMO_API UDCommonUserWidget : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "DViewModel")
	UBaseAttributeViewModel* GetBaseAttributeViewModel() const;
};
