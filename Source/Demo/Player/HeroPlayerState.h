// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DPlayerState.h"
#include "GameplayAbilitySpec.h"
#include "HeroPlayerState.generated.h"

UCLASS()
class DEMO_API AHeroPlayerState : public ADPlayerState
{
	GENERATED_BODY()

public:
	AHeroPlayerState();
	
	virtual void InitAbilitySystem(ADCharacter* Character) override;

	virtual void RegisterAttributes() override;

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerUpdateAbilityLevel(const FGameplayAbilitySpecHandle AbilitySpecHandle);

	UFUNCTION(NetMulticast, Reliable)
	void NetMulticastUpdateAbilityLevel(const FGameplayAbilitySpec& AbilitySpec);

protected:
	void OnXpAttributeChange(const FOnAttributeChangeData& XpData);

	void OnStrengthAttributeChange(const FOnAttributeChangeData& StrengthData, bool bIsExtra);

	void OnIntelligenceAttributeChange(const FOnAttributeChangeData& IntelligenceData, bool bIsExtra);

	void OnAgilityAttributeChange(const FOnAttributeChangeData& AgilityData, bool bIsExtra);

	void OnSpAttributeChange(const FOnAttributeChangeData& SpData, bool bIsUltimate);
};
