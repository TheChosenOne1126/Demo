// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "Animation/AnimInstance.h"
#include "DAnimInstance.generated.h"

UCLASS()
class DEMO_API UDAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	void InitializeTagsByAbilitySystem(UAbilitySystemComponent* Asc);

protected:
	UFUNCTION(BlueprintCallable, Category = "DAnimation", meta = (BlueprintThreadSafe))
	void UpdateForwardAndSpeed();
	
	UPROPERTY(EditDefaultsOnly, Category = "Tag")
	FGameplayTagBlueprintPropertyMap TagPropertyMap;

	UPROPERTY(BlueprintReadOnly, Category = "Speed")
	float ForwardSpeed = 0.f;
	
	UPROPERTY(BlueprintReadOnly, Category = "Speed")
	float RightSpeed = 0.f;
};
