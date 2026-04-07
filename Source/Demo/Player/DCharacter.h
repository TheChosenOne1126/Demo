// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Character.h"
#include "DCharacter.generated.h"

UCLASS(Abstract)
class DEMO_API ADCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void SetPawnTag(const FGameplayTag& NewPawnTag);
	
	const FGameplayTag& GetPawnTag() const;
	
protected:
	UPROPERTY(Replicated)
	FGameplayTag PawnTag;
};
