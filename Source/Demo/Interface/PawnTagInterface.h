// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Interface.h"
#include "PawnTagInterface.generated.h"

UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class UPawnTagInterface : public UInterface
{
	GENERATED_BODY()
};

class DEMO_API IPawnTagInterface
{
	GENERATED_BODY()

public:
	virtual FGameplayTag GetPawnTag() const = 0;
	
	virtual void SetPawnTag(const FGameplayTag& NewPawnTag) = 0;
};
