// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "AttributeSet.h"
#include "GameplayEffectTypes.h"
#include "GameFramework/PlayerState.h"
#include "DPlayerState.generated.h"

class ADCharacter;
class UAbilityDataAsset;
class UBaseAttributeViewModel;
class UBaseAttributeSet;
class UDAbilitySystemComponent;

UCLASS()
class DEMO_API ADPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ADPlayerState();

	virtual void PostInitializeComponents() override;
	
	virtual void PostUnregisterAllComponents() override;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	virtual void OnPawnPossessed(ADCharacter* Character);

	UFUNCTION(NetMulticast, Reliable)
	void NetMulticastShowDamageNumber(float DamageValue, FGameplayTagContainer DamageTags);

	UPROPERTY()
	TObjectPtr<UBaseAttributeViewModel> BaseAttributeViewModel;

protected:
	void OnHpAttributeChange(const FOnAttributeChangeData& HpData);
	
	void OnMaxHpAttributeChange(const FOnAttributeChangeData& MaxHpData);

	void OnHpRegenAttributeChange(const FOnAttributeChangeData& HpRegenData);

	void OnMpAttributeChange(const FOnAttributeChangeData& MpData);

	void OnMaxMpAttributeChange(const FOnAttributeChangeData& MaxMpData);

	void OnMpRegenAttributeChange(const FOnAttributeChangeData& MpRegenData);

	void OnLvAttributeChange(const FOnAttributeChangeData& LvData);
	
	void OnDamageAttributeChange(const FOnAttributeChangeData& DamageData, bool bIsExtra);

	void OnArmorAttributeChange(const FOnAttributeChangeData& ArmorData, bool bIsExtra);

	void OnMagicResistAttributeChange(const FOnAttributeChangeData& MagicResistData, bool bIsExtra);

	void OnAttackSpeedAttributeChange(const FOnAttributeChangeData& AttackSpeedData) const;

	TMap<FGameplayAttribute, FDelegateHandle> AttributeChangeHandleMap;

	UPROPERTY()
	TObjectPtr<UDAbilitySystemComponent> Asc;
};