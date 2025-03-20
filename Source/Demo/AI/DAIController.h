// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Runtime/AIModule/Classes/AIController.h"
#include "DAIController.generated.h"

UCLASS()
class DEMO_API ADAIController : public AAIController
{
	GENERATED_BODY()

public:
	ADAIController();

	virtual void InitPlayerState() override;
};
