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
	UPROPERTY(EditDefaultsOnly, Category = "Tag")
	FGameplayTagBlueprintPropertyMap TagPropertyMap;
};
