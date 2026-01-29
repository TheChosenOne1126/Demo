// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "WidgetInterface.generated.h"

UENUM()
enum class EWidgetZOrder : int32
{
	Default,
	HUD,
	LoadingScreen
};

UINTERFACE(MinimalAPI, Blueprintable)
class UWidgetInterface : public UInterface
{
	GENERATED_BODY()
};

class DEMO_API IWidgetInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent)
	EWidgetZOrder GetZOrder() const;
	virtual EWidgetZOrder GetZOrder_Implementation() const { return EWidgetZOrder::Default; }
};
