// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SweepInterface.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class USweepInterface : public UInterface
{
	GENERATED_BODY()
};

class DEMO_API ISweepInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent)
	UPrimitiveComponent* GetSweptComponent();
	UPrimitiveComponent* GetSweptComponent_Implementation() { return nullptr; }
};
