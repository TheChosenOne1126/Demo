// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UnLuaInterface.h"
#include "Abilities/GameplayAbility.h"
#include "DGameplayAbility.generated.h"

UCLASS(Abstract, HideCategories = (CoolDowns))
class DEMO_API UDGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UDGameplayAbility();
	
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
	
	void OnGameplayTagEvent(const FGameplayTag Tag, int32 Count);
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnInitialized(const FGameplayAbilityActorInfo& ActorInfo);
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnDeInitialized(const FGameplayAbilityActorInfo& ActorInfo);
	
	UPROPERTY()
	FGameplayTagContainer CooldownTags;
	
	UPROPERTY(EditDefaultsOnly, Category = "Tags")
	FGameplayTagContainer ActivationCancelTags;
	
private:
	TMap<FGameplayTag, FDelegateHandle> ActivationCancelTagToHandleMap;
};
