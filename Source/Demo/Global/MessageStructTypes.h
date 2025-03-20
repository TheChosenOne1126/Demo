// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MessageStructTypes.generated.h"

USTRUCT()
struct FMessageFloatStructType
{
	GENERATED_BODY()

	UPROPERTY()
	float Value = 0.f;
};