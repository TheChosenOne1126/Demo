// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "DCharacter.generated.h"

UCLASS(Abstract)
class DEMO_API ADCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	virtual void PossessedBy(AController* NewController) override;

	virtual void OnRep_PlayerState() override;
};
