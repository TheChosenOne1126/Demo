// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interface/PawnTagInterface.h"
#include "Runtime/AIModule/Classes/AIController.h"
#include "DAIController.generated.h"

UCLASS()
class DEMO_API ADAIController : public AAIController, public IPawnTagInterface
{
	GENERATED_BODY()

public:
	ADAIController();

	virtual void InitPlayerState() override;
	
	virtual FGameplayTag GetPawnTag() const override;
	
	virtual void SetPawnTag(const FGameplayTag& NewPawnTag) override;
	
private:
	UPROPERTY()
	FGameplayTag PawnTag;
};
