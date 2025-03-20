// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DPlayerState.h"
#include "GameplayAbilitySpecHandle.h"
#include "HeroPlayerState.generated.h"

class UHeroAttributeViewModel;

UCLASS()
class DEMO_API AHeroPlayerState : public ADPlayerState
{
	GENERATED_BODY()

public:
	AHeroPlayerState();
	
	virtual void PostInitializeComponents() override;

	virtual void OnPawnPossessed(ADCharacter* Character) override;

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerUpdateAbilityLevel(FGameplayAbilitySpecHandle AbilitySpecHandle);

	UFUNCTION(NetMulticast, Reliable)
	void NetMulticastUpdateAbilityLevel(FGameplayAbilitySpecHandle AbilitySpecHandle);
	
	UPROPERTY()
	TObjectPtr<UHeroAttributeViewModel> HeroAttributeViewModel;

protected:
	void OnXpAttributeChange(const FOnAttributeChangeData& XpData);

	void OnStrengthAttributeChange(const FOnAttributeChangeData& StrengthData, bool bIsExtra);

	void OnIntelligenceAttributeChange(const FOnAttributeChangeData& IntelligenceData, bool bIsExtra);

	void OnAgilityAttributeChange(const FOnAttributeChangeData& AgilityData, bool bIsExtra);

	void OnSpAttributeChange(const FOnAttributeChangeData& SpData, bool bIsUltimate);
};
