// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UnLuaInterface.h"
#include "Abilities/GameplayAbility.h"
#include "DGameplayAbility.generated.h"

UCLASS(Abstract)
class DEMO_API UDGameplayAbility : public UGameplayAbility, public IUnLuaInterface
{
	GENERATED_BODY()

public:
	UDGameplayAbility();
	
	virtual FString GetModuleName_Implementation() const override;
	
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

	virtual void OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	
	virtual bool CheckCooldown(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;

	virtual void ApplyCooldown(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo) const override;

	virtual bool CheckCost(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;

	virtual void ApplyCost(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo) const override;
	
	virtual UGameplayEffect* GetCooldownGameplayEffect() const override;
	
protected:
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled) override;
	
	UFUNCTION(BlueprintImplementableEvent, meta = (BlueprintInternalUseOnly = "true"))
	float LuaObtainCooldownMagnitude() const;
	
	UPROPERTY()
	FGameplayTagContainer CooldownTags;
	
	UFUNCTION(BlueprintImplementableEvent, meta = (BlueprintInternalUseOnly = "true"))
	bool LuaObtainCostInfo(TMap<FGameplayTag, float>& OutCostInfo) const;
	
	UFUNCTION(BlueprintImplementableEvent, meta = (BlueprintInternalUseOnly = "true"))
	void OnInitialized(const FGameplayAbilityActorInfo& ActorInfo);
	
	UFUNCTION(BlueprintImplementableEvent, meta = (BlueprintInternalUseOnly = "true"))
	void OnDeInitialized(const FGameplayAbilityActorInfo& ActorInfo);
};
