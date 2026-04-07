// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "UnLuaInterface.h"
#include "GameFramework/PlayerState.h"
#include "DPlayerState.generated.h"

class UDAbilitySystemComponent;

UCLASS()
class DEMO_API ADPlayerState : public APlayerState, public IAbilitySystemInterface, public IUnLuaInterface
{
	GENERATED_BODY()

public:
	ADPlayerState();
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	virtual FString GetModuleName_Implementation() const override;

	UFUNCTION(NetMulticast, Reliable)
	void NetMulticastShowDamageNumber(float DamageValue, FGameplayTagContainer DamageTags);

protected:
	UPROPERTY()
	TObjectPtr<UDAbilitySystemComponent> Asc;
};